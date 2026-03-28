#pragma once

#include <unordered_map>

#include "parser/parsed_type.hpp"
#include "commands/commands.hpp"

namespace COMMAND{

    inline PARSER::PARSED parse_ping(COMMAND::RAW& ping)
    {
        return COMMAND::PING{ping.arr_char_length};
    }

    inline PARSER::PARSED parse_set(COMMAND::RAW& set)
    {
        COMMAND::STRING::SET set_realized{};

        if( set.command_list.size() < 2){
            return PARSER::PARSE_ERROR::MALFORMED_COMMAND;
        }

        set_realized.key = set.command_list[1];
        set_realized.val = set.command_list[2];
        set_realized.arr_char_length = set.arr_char_length;

        return set_realized;
    }

    inline PARSER::PARSED parse_get(COMMAND::RAW &get){

        COMMAND::STRING::GET get_realized{};
        if ( get.command_list.size() < 1 ){
            return PARSER::PARSE_ERROR::MALFORMED_COMMAND;
        }

        get_realized.key = get.command_list[1];
        get_realized.arr_char_length = get.arr_char_length;

        return get_realized;
    }

    using CommandHolder = PARSER::PARSED(*)(COMMAND::RAW&);

    inline std::unordered_map<std::string_view, CommandHolder> name_to_parsed = {
       { "GET" , parse_get},
       { "SET" , parse_set},
       { "PING", parse_ping}
    };

    inline PARSER::PARSED command_factory(COMMAND::RAW& raw_command)
    {

        if(raw_command.command_list.empty()){
            return PARSER::PARSE_ERROR::MALFORMED_COMMAND;//TODO: Is this even possible?
        }

        auto check = name_to_parsed.find(raw_command.command_list[0]);

        if(check == name_to_parsed.end()){
            return PARSER::PARSE_ERROR::MALFORMED_COMMAND;
        }

        return check->second(raw_command);

    }

}