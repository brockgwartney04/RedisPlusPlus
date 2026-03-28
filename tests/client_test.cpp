#include "tests/doctest.hpp"
#include "client/connections_container.hpp"

TEST_CASE("Connections Container") {
    Connections::force_get(1);
    auto client2 = Connections::force_get(2);
    client2.buffer = "Hello, World!";

    CHECK(Connections::count() == 2);

    Connections::del(2);
    CHECK(Connections::count() == 1);

    client2 = Connections::force_get(2);
    CHECK(client2.buffer == ""); // Should be empty since it's a new client
}

