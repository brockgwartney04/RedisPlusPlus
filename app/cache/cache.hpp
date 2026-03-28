/*
    The Actual Cache Data Structure.
*/
#pragma once

#include <unordered_map>
#include <string>
#include "parser/response.hpp"

namespace CACHE{

    enum class MESSAGE {
        MISS,
        ALREADY_IN,
        GOOD,
        BAD
    };

    struct RVAL{ //For Simplicity - We Assume Only Strings are Being Used in our Redis Clone
        std::string value; 
        std::string response;
        CACHE::MESSAGE message;
        size_t command_char_length;
    };

    static std::unordered_map<std::string, std::string> cache;

    inline RVAL add(std::string key, std::string value, size_t length){

        RVAL rval; 

        cache[key] = value;

        rval.command_char_length = length;
        rval.message = MESSAGE::GOOD;
        rval.response = "+OK\r\n";

        return rval; 

    }

    inline RVAL update(std::string key, std::string value, size_t length){
        RVAL rval; 
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            cache[key] = value;
        }
        else{
            rval.message = MESSAGE::MISS;
        }

        rval.command_char_length = length;

        return rval;
        
    }
    
    inline RVAL get(std::string key, size_t length){
        RVAL rval; 
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            rval.value = cache[key];
            rval.response = PARSER::format(rval.value);
        }
        else{
            rval.message = MESSAGE::MISS;
            rval.response = "$-1\r\n";
        }

        rval.command_char_length = length;

        return rval;
    }

    inline RVAL del(std::string key, size_t length){
        RVAL rval;
        if(cache.contains(key)){
            rval.message = MESSAGE::GOOD;
            cache.erase(key);
        }
        else{
            rval.message = MESSAGE::MISS;
        }

        rval.command_char_length = length;

        return rval;
    }

}