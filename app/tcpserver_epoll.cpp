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
        std::cout << "Got FD!\n";

        //Tell OS to Set Options on the Socket
        int flags = fcntl(fd, F_GETFL, 0);
        if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) return -1;
        std::cout << "Set Flags!\n";

        //More or less the same
        int opt = 1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) return -1;
        if(setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1) return -1;
        std::cout << "Set Options!\n";

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(1234); //Port number
        addr.sin_addr.s_addr = htonl(0); //Any IP is good. 

        if(bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1) return -1;
        std::cout<< "Binded\n";

        return fd;

    }

    int process_request(int client_fd, int epoll_fd){

        char buffer[1024];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes < 0){
            std::cerr << "recv";
        }
        else{
            buffer[bytes] = '\0';
            std::cout << "Received" << buffer << std::endl;
        }

        const char* response = "Message Recieved";
        send(client_fd, response, strlen(response), 0);

        epoll_ctl(epoll_fd, EPOLL_CTL_DEL ,client_fd, nullptr);
        close(client_fd);
        return 1;
    }

    #define MAX_CONENCT 100

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

        while(true){
            amt = epoll_wait(epoll_fd, events, MAX_CONENCT, -1);
            if(amt == -1) ; /// TODO: Handle Error Here? Not entirley Sure.

            for (int i {0}; i < amt; i++) {

                if(events[i].data.fd == socket_fd){ // New Connection
                    new_socket = accept(socket_fd, nullptr, nullptr);
                    if (new_socket == -1) std::cout<<"Failed to Accept Connection on " << events[i].data.fd;

                    epoll_event new_ev{};
                    new_ev.events = EPOLLIN;
                    new_ev.data.fd = new_socket;
                    
                    fcntl(new_socket, F_SETFL, O_NONBLOCK);
                    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &new_ev) == -1) std::cout<< "Failed to add to EPOLL: " << events[i].data.fd;

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