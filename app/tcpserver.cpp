#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>

#include <iostream>
#include <cstring>

const int CONNECTION_LIMIT = 5;

int main(){

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0){
        std::cerr<<"Error: Failed to Get Socket";
        return 1;
    }


    int val = 1;
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0){
        std::cerr <<"Error: Failed to setsockopt";
        return 1;
    }

    struct sockaddr_in addr = {};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    if(bind(server_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
        std::cerr<<"Error: Bind";
        close(server_fd);
        return 1;
    }

    if(listen(server_fd, CONNECTION_LIMIT) < 0){
        std::cerr<<"Error: listen";
        close(server_fd);
        return 1;
    }

    std::cout << "Listening on " << ntohs(addr.sin_port);
    int total_count {0};

    while (true) {
        int client_fd = accept(server_fd, nullptr, nullptr);
        if(client_fd < 0){
            std::cerr << "Error: accept";
            continue;
        }

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

        close(client_fd);
        total_count += 1;
    }

    close(server_fd);

    return 0;

}



