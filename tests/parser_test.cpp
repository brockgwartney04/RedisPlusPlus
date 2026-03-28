#include "tests/doctest.hpp"
#include "parser/parser.hpp"

//parse_line
TEST_CASE("parse_line - valid"){
    std::string_view buffer = "*3\r\n";
    auto result = PARSER::parse_line(buffer);
    REQUIRE(std::holds_alternative<PARSER::Line_Info>(result));
    auto line_info = std::get<PARSER::Line_Info>(result);
    REQUIRE(line_info.line_length == 4);
    REQUIRE(line_info.cur_line == "*3");
}

TEST_CASE("parse_line - short"){
    std::string_view buffer = "*3\n";
    auto result = PARSER::parse_line(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::INCOMPLETE);
}
//parse_length
TEST_CASE("parse_length - valid"){
    std::string_view buffer = "*3\r\n";
    PARSER::Line_Info line_info = std::get<PARSER::Line_Info>(PARSER::parse_line(buffer));
    auto result = PARSER::parse_length(line_info);
    REQUIRE(std::holds_alternative<int>(result));
    REQUIRE(std::get<int>(result) == 3);
}

TEST_CASE("parse_length - non-numeric"){
    std::string_view buffer = "*x\r\n";
    PARSER::Line_Info line_info = std::get<PARSER::Line_Info>(PARSER::parse_line(buffer));
    auto result = PARSER::parse_length(line_info);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::MALFORMED_COMMAND);
}

//parse_string
TEST_CASE("parse_string - valid"){
    std::string_view buffer = "$5\r\nHello\r\n";
    auto result = PARSER::parse_string(buffer);
    REQUIRE(std::holds_alternative<PARSER::Line_Info>(result));
    auto line_info = std::get<PARSER::Line_Info>(result);
    REQUIRE(line_info.line_length == 11); // $5\r\nHello\r\n
    REQUIRE(line_info.cur_line == "Hello");
}

TEST_CASE("parse_string - incomplete"){
    std::string_view buffer = "$5\r\nHel";
    auto result = PARSER::parse_string(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::INCOMPLETE);
}

TEST_CASE("parse_string - malformed"){
    std::string_view buffer = "$1\r\nHellotest\n";
    auto result = PARSER::parse_string(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::MALFORMED_COMMAND);
}
//parse_array
TEST_CASE("parse_array - valid"){
    std::string_view buffer = "*2\r\n$3\r\nSET\r\n$3\r\nkey\r\n";
    auto result = PARSER::parse_array(buffer);
    REQUIRE(std::holds_alternative<COMMAND::STRING::SET>(result));
    auto command = std::get<COMMAND::STRING::SET>(result);
}

TEST_CASE("parse_array - incomplete"){
    std::string_view buffer = "*2\r\n$3\r\nSET\r\n$3\r\nke";
    auto result = PARSER::parse_array(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::INCOMPLETE);
}

TEST_CASE("parse_array - malformed 1"){
    std::string_view buffer = "$2\r\n$3\r\nSET\r\n$3\r\nkey";
    auto result = PARSER::parse_array(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::MALFORMED_COMMAND);
}

TEST_CASE("parse_array - malformed 2"){
    std::string_view buffer = "*2\r\n$1\r\nSET\r\n$3\r\nkey\r\n";
    auto result = PARSER::parse_array(buffer);
    REQUIRE(std::holds_alternative<PARSER::PARSE_ERROR>(result));
    REQUIRE(std::get<PARSER::PARSE_ERROR>(result) == PARSER::PARSE_ERROR::MALFORMED_COMMAND);
}
