//Implementation of RESP2 PROTOCOL (ish)
#pragma once
#include <string>
#include <vector>

namespace COMMAND{

    struct RAW { // Raw commands are the literal array that the command came in as.
        std::vector<std::string_view> command_list;
        size_t arr_char_length; // The actual char count between the strt of the arr -> to the end
    };

    struct PING{
        size_t arr_char_length;
    };

    namespace STRING{

        struct SET {
            std::string key;
            std::string val;
            size_t arr_char_length;
        };

        struct GET {
            std::string key;
            size_t arr_char_length;
        };


    }


}