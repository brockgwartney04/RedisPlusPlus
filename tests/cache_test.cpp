#include "tests/doctest.hpp"
#include "cache/cache.hpp"
#

TEST_CASE("Cache Add and Get") {
    CACHE::RVAL add_result = CACHE::add("key1", "value1", 10);
    CHECK(add_result.message == CACHE::MESSAGE::GOOD);
    CHECK(add_result.response == "+OK\r\n");

    CACHE::RVAL get_result = CACHE::get("key1", 10);
    CHECK(get_result.message == CACHE::MESSAGE::GOOD);
    CHECK(get_result.value == "value1");
}

TEST_CASE("Cache Get Miss") {
    CACHE::RVAL get_result = CACHE::get("nonexistent_key", 10);
    CHECK(get_result.message == CACHE::MESSAGE::MISS);
    CHECK(get_result.response == "$-1/r/n");
}

TEST_CASE("Cache Update Hit and Miss") {
    CACHE::RVAL get_result = CACHE::update("l", "value1", 10);
    CHECK(get_result.message == CACHE::MESSAGE::MISS);
    get_result = CACHE::update("key1", "new_value", 10);
    CHECK(get_result.message == CACHE::MESSAGE::GOOD);
    CHECK(CACHE::get("key1", 10).value == "new_value");
}

