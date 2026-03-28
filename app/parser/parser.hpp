#pragma once

#include <string>
#include <variant>
#include <charconv>
#include <iostream>

#include "commands/commands.hpp"
#include "commands/command_factory.hpp"
#include "parser/parsed_type.hpp"
#include "helpers/debug_print.hpp"

namespace PARSER {

    struct Line_Info {
        std::string_view cur_line; // Everything prior to '/r/n'
        signed int line_length; //cur_line.size() + '/r/n'
    };

    using PARSED_LINE = std::variant<Line_Info, PARSE_ERROR>;

    /**
     * Given The Current Buffer of the Client
     * Read Up until the next '/r/n'
     * Return the line info or a Parse_ERROR  
     */
    PARSED_LINE parse_line(std::string_view buffer){
        //EX : "*3\r\n$3\r\nSET\r\n$3\r\nkey\r\n$5\r\nvalue\r\n*2\r\n$3\r\nGET\r\n$3\r\nkey\r\n"

        //Invalid State - Theoritcally should have ATLEAST '/r/n' ; however im not sure if we should check/account for stuff prior 
        if(buffer.size() < 2){
            return PARSE_ERROR::INCOMPLETE;
        }

        for(int i{1}; i < buffer.size(); i++){
            if(buffer[i-1] == '\r' and buffer[i] == '\n'){
                Line_Info info;
                info.cur_line = buffer.substr(0, i - 1); //TODO : Check if this is right?
                info.line_length = i + 1;
                return info;
            }
        }

        return PARSE_ERROR::INCOMPLETE;

    }


    using PARSED_LENGTH = std::variant<int, PARSE_ERROR>;

    /**
        Given a a line, get the length of the array/string
        TODO: Have some sort of system to determine if the char is correct? but idk.
    */
    PARSED_LENGTH parse_length(Line_Info line){

        if( /*line.cur_line[0] != '*' ||*/ line.cur_line.size() < 2){
            return PARSE_ERROR::INCOMPLETE;
        } 

        int arr_length {0};
        auto check = std::from_chars(line.cur_line.data() + 1, line.cur_line.data() + line.cur_line.size(), arr_length); //+1 to account for * 
        
        if(check.ec != std::errc{}){ //We were not able to Extract the Size
            return PARSE_ERROR::MALFORMED_COMMAND;
        }

        return arr_length;

    }

    using PARSED_STRING = std::variant<Line_Info, PARSE_ERROR>;
    /**
     *  Given a buffer, that represents a string, parse it
     *  Returns a line info with the actual string (nothing with the length)
     *  And returns a the length of the two lines combined
     */
    PARSED_STRING parse_string(std::string_view buffer){
        
        auto length_line = parse_line(buffer);

        if(std::holds_alternative<PARSE_ERROR>(length_line)){
            PARSE_ERROR err = std::get<PARSE_ERROR>(length_line);
            return err;
        }
        
        Line_Info length_line_info = std::get<Line_Info>(length_line);
        auto length_check = parse_length(length_line_info);

        if(std::holds_alternative<PARSE_ERROR>(length_check)){
            PARSE_ERROR err = std::get<PARSE_ERROR>(length_check);
            return err; 
        }

        int length = std::get<int>(length_check);

        //We have the length now, we can create a string view that represents the second line.

        if(buffer.size() == length_line_info.line_length){ //We cannot safely create the offset string_view
            return PARSE_ERROR::INCOMPLETE;
        }

        std::string_view string_line = buffer.substr(length_line_info.line_length);
        //Now we need to check if the length is possible.
        if(string_line.size() < length + 2){
            return PARSE_ERROR::INCOMPLETE;
        }

        //We cannot use the parse_line function as we know the length AND there may be /r/n in the string itself - so - parse_line might break

        if(string_line[length] != '\r' || string_line[length + 1] != '\n'){
            return PARSE_ERROR::MALFORMED_COMMAND;
        }

        Line_Info string_info; 

        string_info.line_length = length_line_info.line_length + length + 2;
        string_info.cur_line = string_line.substr(0, length);

        return string_info;

    }

    PARSED parse_array(std::string_view buffer){

        if(buffer.size() == 0){
            return PARSE_ERROR::DONE;
        }

        if(buffer[0] != '*'){
            return PARSE_ERROR::MALFORMED_COMMAND;
        }

        auto length_line = parse_line(buffer);

        if(std::holds_alternative<PARSE_ERROR>(length_line)){
            return std::get<PARSE_ERROR>(length_line);
        }

        auto line_info = std::get<Line_Info>(length_line);
        auto length_check = parse_length(line_info);

        if(std::holds_alternative<PARSE_ERROR>(length_check)){
            return std::get<PARSE_ERROR>(length_check);
        }

        size_t length = std::get<int>(length_check);
        
        COMMAND::RAW command;
        size_t offset = line_info.line_length;
        size_t total_length = offset;
        std::string_view cur_view = buffer;

        for(int i {0}; i < length; i++){            
            
            if(offset > cur_view.size()){
                return PARSE_ERROR::INCOMPLETE;
            }

            cur_view = cur_view.substr(offset);            
            auto string_check = parse_string(cur_view);

            if(std::holds_alternative<PARSE_ERROR>(string_check)){
                return std::get<PARSE_ERROR>(string_check);
            }

            Line_Info string_info = std::get<Line_Info>(string_check);

            command.command_list.push_back(string_info.cur_line);

            offset = string_info.line_length;
            total_length += offset;
        }

        command.arr_char_length = total_length;
        return COMMAND::command_factory(command);

    }

}
