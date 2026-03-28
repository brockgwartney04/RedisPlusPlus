#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

#include "client/connections_container.hpp"
#include "parser/parser.hpp"
#include "execute/executor.hpp"
#include "helpers/debug_print.hpp"

enum class HANDLE : int {
    FINISHED,
    INCOMPLETE,
    ERROR
};
    
void ret_error(std::string err_msg){
    std::cerr<< err_msg;
    exit(1);
}

/**
 * Create and initilize a IPV4 TCP socket - Port is Binded BUT NOT LISTENING
 * @return -1 if error else open socket
 */
int init_socket(){

    //Ask OS for Socket (number)
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd == -1) return -1;

    //Tell OS to Set Options on the Socket
    int flags = fcntl(fd, F_GETFL, 0);
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;

    //More or less the same
    int opt = 1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) return -1;
    if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) return -1;

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234); //Port number
    addr.sin_addr.s_addr = htonl(0); //Any IP is good. 

    if(bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) return -1;

    return fd; 

}

void get_buffer(Client& client){
    
    char buffer[1024];
    std::string n_buffer;

    while(true){
        ssize_t bytes = recv(client.fd, buffer, sizeof(buffer) - 1, 0);
        if(bytes > 0){ // apppend to buffer
            client.buffer.append(buffer, bytes);
        }
        else if(bytes == 0){ //Nothing Left to proccess
            client.good_to_close = true;
            break; //Connection is Closed.
        }
        else if (errno == EAGAIN || errno == EWOULDBLOCK){
            break; //Nothing Left to read
        }
        else{
            //TOOD:: HANDLE ERROR
            ret_error("RECV");
        }

    }
    //TODO if this returns 
    return;
    
}

void handle_error(PARSER::PARSE_ERROR error){
    return;
}

void close_connection(int client_fd, int epoll_fd){
    Connections::del(client_fd);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL ,client_fd, nullptr);
    close(client_fd);
}

HANDLE process_request(int client_fd, int epoll_fd)
{
    Client& client = Connections::force_get(client_fd);
    get_buffer(client);

    //TODO : bounds check.
    while (true) {
        
        auto result = PARSER::parse_array(client.buffer);

        if(std::holds_alternative<PARSER::PARSE_ERROR>(result)){
            PARSER::PARSE_ERROR error = std::get<PARSER::PARSE_ERROR>(result);

            if(error == PARSER::PARSE_ERROR::INCOMPLETE){ 
                return HANDLE::INCOMPLETE; //Todo - Figure out what to do here. moreso, what to return.
            }
            
            if(error == PARSER::PARSE_ERROR::DONE && client.good_to_close){
                break;
            }
            else if (error == PARSER::PARSE_ERROR::DONE){
                return HANDLE::INCOMPLETE;
            }

            //BAD DATA!!
            PARSER::respond(client, "-ERR internal error\r\n");
            close_connection(client_fd, epoll_fd);
            return HANDLE::ERROR;

        }

        auto executed = EXECUTOR::execute(result);
        debug_print(executed.response);
        PARSER::respond(client, executed.response);
        client.buffer = client.buffer.erase(0,executed.command_char_length);
    }
    
    return HANDLE::FINISHED;
}

#define MAX_CONENCT 10000

int main(){
    //Since Linux 2.6.8, the size argument is ignored, but must be greater than zero;
    int epoll_fd = epoll_create(1);
    if(epoll_fd == -1) ret_error("Failed to Open epoll");

    int socket_fd = init_socket();
    if(socket_fd == -1) ret_error("Failed to Initlize Socket!");

    if(listen(socket_fd, SOMAXCONN) < 0) ret_error("Could not Listen to Socket");
    
    static struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = socket_fd;
    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, socket_fd, &ev) == -1) ret_error("Could Not Create EPOLL");

    struct epoll_event events[MAX_CONENCT];
    int amt, new_socket; 
    
    std::cout<<"<<<<<< SERVER RUNNING <<<<<<"<<std::endl;

    while(true){
        amt = epoll_wait(epoll_fd, events, MAX_CONENCT, -1);
        if(amt == -1) ; /// TODO: Handle Error Here? Not entirley Sure.
        for (int i {0}; i < amt; i++) {
            if(events[i].data.fd == socket_fd){ // New Connection
                new_socket = accept(socket_fd, nullptr, nullptr);
                if (new_socket == -1) return -1;

                epoll_event new_ev{};
                new_ev.events = EPOLLIN;
                new_ev.data.fd = new_socket;
                
                fcntl(new_socket, F_SETFL, O_NONBLOCK);
                if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &new_ev) == -1) return -1;

            }
            else{  //Data is Ready.
                process_request(events[i].data.fd, epoll_fd);
            }

        }

    }


    close(epoll_fd);
    close(socket_fd);
    return 0;
}