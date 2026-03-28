#pragma once
#include "cache/cache.hpp"
#include "commands/commands.hpp"
#include "parser/parser.hpp"



namespace EXECUTOR{
    struct Dispatch{
        CACHE::RVAL operator()(const COMMAND::STRING::SET command) const {
            return CACHE::add(command.key, command.val, command.arr_char_length);
        };
        CACHE::RVAL operator()(const COMMAND::STRING::GET command) const {
            return CACHE::get(command.key, command.arr_char_length);
        };
        CACHE::RVAL operator()(const COMMAND::PING command) const {
            return CACHE::RVAL{"", PARSER::format("PONG"), CACHE::MESSAGE::GOOD,command.arr_char_length};
        };
        
        //TOdo Remove after all commands added.
        template <typename T>
        CACHE::RVAL operator()(const T&) const {
            return CACHE::RVAL{ "ERR unknown command","", CACHE::MESSAGE::BAD };
        }
    };

    CACHE::RVAL execute(PARSER::PARSED& command){
        return std::visit(Dispatch{}, command);
    }
}

