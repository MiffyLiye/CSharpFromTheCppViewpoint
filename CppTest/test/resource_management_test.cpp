#include "../include/catch.hpp"
#include "finally.hpp"

using namespace std;
using namespace tcpppl;

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
    Resource(Holder& holder)
    {
        this->p_holder = &holder;
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
        auto resource = Resource(holder);
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("scope guard", "[resource management]")
{
    auto holder = Holder();
    {
        holder.count += 1;
        auto act = finally([&]() { holder.count -= 1; });
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("unique pointer: exclusive ownership", "[resource management]")
{
    auto holder = Holder();
    {
        auto pr = make_unique<Resource>(holder);
        REQUIRE(holder.count == 1);
    }
    REQUIRE(holder.count == 0);
}

TEST_CASE("shared pointer: shared ownership", "[resource management]")
{
    auto holder = Holder();
    auto pr = make_shared<Resource>(holder);
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
        auto pr1 = make_shared<Resource>(holder);
        auto pr2 = make_shared<Resource>(holder);
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
        auto pr1 = make_shared<Resource>(holder);
        auto pr2 = make_shared<Resource>(holder);
        pr1->shared = pr2;
        pr2->weak = pr1;
        REQUIRE(holder.count == 2);
    }
    REQUIRE(holder.count == 0);
}
