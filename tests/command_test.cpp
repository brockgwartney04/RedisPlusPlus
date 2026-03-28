#include "tests/doctest.hpp"
#include "commands/command_factory.hpp"

TEST_CASE("command_factory - valid SET"){
    COMMAND::RAW command;
    command.command_list = {"SET", "key", "value"};
    auto result = COMMAND::command_factory(command);
    REQUIRE(std::holds_alternative<COMMAND::STRING::SET>(result));
    auto set_command = std::get<COMMAND::STRING::SET>(result);
    REQUIRE(set_command.key == "key");
    REQUIRE(set_command.val == "value");
}

TEST_CASE("command_factory - valid GET"){
    COMMAND::RAW command;
    command.command_list = {"GET", "key"};
    auto result = COMMAND::command_factory(command);
    REQUIRE(std::holds_alternative<COMMAND::STRING::GET>(result));
    auto get_command = std::get<COMMAND::STRING::GET>(result);
    REQUIRE(get_command.key == "key");
}

TEST_CASE("command_factory - VALID PING"){
    COMMAND::RAW command;
    command.command_list = {"PING"};
    auto result = COMMAND::command_factory(command);
    REQUIRE(std::holds_alternative<COMMAND::PING>(result));
}

TEST_CASE("command_factory - invalid command"){
    COMMAND::RAW command;
    command.command_list = {"INVALID"};
    auto result = COMMAND::command_factory(command);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::MALFORMED_COMMAND);
}

