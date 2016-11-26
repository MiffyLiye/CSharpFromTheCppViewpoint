#include "../include/catch.hpp"
#include <functional>

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
    auto act = [&] { n = m; };

    act();
    REQUIRE(n == string{"May"});

    m = string{"June"};

    act();
    REQUIRE(n == string{"June"});
}

TEST_CASE("capture by value again", "[lambda]")
{
    auto m = string{"May"};
    auto n = string{""};
    auto act = [m, &n] { n = m; };

    m = string{"June"};
    act();

    REQUIRE(n == string{"May"});
}

TEST_CASE("capture for loop variable by value", "[lambda]")
{
    auto months = vector<string>{"May", "June"};
    auto n = string{""};
    auto acts = vector<function<void()>>{};

    for (size_t i = 0; i < 1; i++)
    {
        acts.push_back([&, i] { n = months[i]; });
    }
    acts[0]();

    REQUIRE(n == string{"May"});
}

TEST_CASE("capture for loop variable by reference to outer scope", "[lambda]")
{
    auto months = vector<string>{"May", "June"};
    auto n = string{""};
    auto acts = vector<function<void()>>{};

    for (size_t i = 0; i < 1; i++)
    {
        acts.push_back([&] { n = months[i]; });
    }
//    acts[0]();

//    UNDEFINED(n == string{"May"});
}

TEST_CASE("foreach loop", "[lambda]")
{
    auto months = vector<string>{"May", "June"};
    auto n = string{""};
    auto acts = vector<function<void()>>{};

    for (auto& m : months)
    {
        acts.push_back([&, m] { n = m; });
    }
    acts[0]();

    REQUIRE(n == string{"May"});
}

TEST_CASE("capture foreach loop variable by reference to outer scope", "[lambda]")
{
    auto months = vector<string>{"May", "June"};
    auto n = string{""};
    auto acts = vector<function<void()>>{};

    for (auto m : months)
    {
        acts.push_back([&] { n = m; });
    }
//    acts[0]();

//    UNDEFINED(n == string{"May"});
}
