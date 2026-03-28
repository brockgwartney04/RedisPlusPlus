#pragma once

#include <variant>

#include "commands/commands.hpp"

namespace PARSER{

    enum class PARSE_ERROR : int{
        GENERAL,
        PARSE_LENGTH,
        MALFORMED_COMMAND,
        INVALID_TYPE,
        INCOMPLETE,
        TOO_LONG,
        DONE 
    };

    using PARSED = std::variant<
        COMMAND::STRING::GET,
        COMMAND::STRING::SET,
        COMMAND::PING,
        COMMAND::RAW,
        PARSE_ERROR
    >;

}