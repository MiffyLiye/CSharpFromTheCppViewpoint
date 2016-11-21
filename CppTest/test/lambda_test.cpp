#include "../include/catch.hpp"

using namespace std;

TEST_CASE("capture by reference", "[lambda]")
{
    auto m = string{"May"};
    auto act = [&]() { m = string{"June"}; };

    act();

    REQUIRE(m == string{"June"});
}

TEST_CASE("capture by value", "[lambda]")
{
    auto m = string{"May"};
    auto act = [=]() mutable { m = string{"June"}; };

    act();

    REQUIRE(m == string{"May"});
}

TEST_CASE("capture by reference again", "[lambda]")
{
    auto m = string{"May"};
    auto n = string{""};
    auto act = [&]() { n = m; };

    m = string{"June"};
    act();

    REQUIRE(n == string{"June"});
}

TEST_CASE("capture by value again", "[lambda]")
{
    auto m = string{"May"};
    auto n = string{""};
    auto act = [m, &n]() { n = m; };

    m = string{"June"};
    act();

    REQUIRE(n == string{"May"});
}
