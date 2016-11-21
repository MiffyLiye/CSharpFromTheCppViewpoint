#include "../include/catch.hpp"
#include "finally.hpp"

using namespace std;

class Holder
{
public:
    int count;

    Holder() : count{0} {}
};

class Resource
{
private:
    Holder* p_holder;
public:
    Resource(Holder* p_holder)
    {
        this->p_holder = p_holder;
        p_holder->count += 1;
    }

    ~Resource() { this->p_holder->count -= 1; }

    shared_ptr<Resource> shared;
    weak_ptr<Resource> weak;
};

TEST_CASE("Resource Acquisition Is Initialization (RAII)", "[resource management]")
{
    auto holder = Holder();
    {
        auto resource = Resource(&holder);
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("scope guard", "[resource management]")
{
    auto holder = Holder();
    {
        holder.count = true;
        auto act = finally([&]() { holder.count -= 1; });
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("unique pointer: exclusive ownership", "[resource management]")
{
    auto holder = Holder();
    {
        auto pr = unique_ptr<Resource>{new Resource(&holder)};
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("shared pointer: shared ownership", "[resource management]")
{
    auto holder = Holder();
    auto pr = shared_ptr<Resource>{new Resource(&holder)};
    auto pq = pr;
    REQUIRE(holder.count == 1);

    pr.reset();
    REQUIRE(holder.count == 1);

    pq.reset();
    REQUIRE(holder.count == 0);
}

TEST_CASE("shared pointer: circular reference", "[resource management]")
{
    auto holder = Holder();
    {
        auto pr1 = shared_ptr<Resource>{new Resource(&holder)};
        auto pr2 = shared_ptr<Resource>{new Resource(&holder)};
        pr1->shared = pr2;
        pr2->shared = pr1;
        REQUIRE(holder.count == 2);
    }
    REQUIRE(holder.count == 2);
}

TEST_CASE("weak pointer: break loop", "[resource management]")
{
    auto holder = Holder();
    {
        auto pr1 = shared_ptr<Resource>{new Resource(&holder)};
        auto pr2 = shared_ptr<Resource>{new Resource(&holder)};
        pr1->shared = pr2;
        pr2->weak = pr1;
        REQUIRE(holder.count == 2);
    }
    REQUIRE(holder.count == 0);
}
