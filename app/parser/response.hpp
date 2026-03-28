#pragma once
#include <string>
#include <format>
#include <sys/socket.h>

#include "client/connections_container.hpp"

/*
    We Assume All Outputs are for Strings 
*/

namespace PARSER{

    //TODO: Could be more performant 
    std::string format(std::string_view value){
        return std::format("${}\r\n{}\r\n", value.size(), value);
    }

    bool respond(Client& client, std::string_view response){
        send(client.fd, response.data(), response.size(), MSG_NOSIGNAL);
        return true; //TODO: this needs to be done better
    }
}






