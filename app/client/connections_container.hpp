#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <iostream>

struct Client {
    int fd = -1;
    std::string buffer = ""; //The Current Buffer
    bool good_to_close = false;
};

namespace Connections{
    static std::unordered_map<int,Client> connections;

    /*Returns new client if exists else, creates new*/
    inline Client& force_get(int fd) {
        Client& client = connections[fd];
        if(client.fd == -1){
            client.fd = fd;
        }
        return client;
    }   

    inline void del(int fd){
        connections.erase(fd);
    }

    inline int count(){
        return connections.size();
    }

}