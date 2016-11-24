#include "../include/catch.hpp"
#include "finally.hpp"

using namespace std;
using namespace tcpppl;

class IPoint
{
public:
    virtual ~IPoint() = default;

    virtual void change(int x, int y) = 0;
};

class Point : public IPoint
{
public:
    int x;
    int y;

    Point(int xx, int yy)
    {
        x = xx;
        y = yy;
    }

    void change(int xx, int yy) override
    {
        x = xx;
        y = yy;
    }
};

TEST_CASE("copy by value", "[value reference]")
{
    auto p = Point{0, 0};
    auto q = p;

    p.x = 1;

    REQUIRE(q.x == 0);
}

TEST_CASE("copy by reference", "[value reference]")
{
    auto p = Point{0, 0};
    auto& q = p;

    p.x = 1;

    REQUIRE(q.x == 1);
}

TEST_CASE("pass parameter by const reference", "[value reference]")
{
    auto p = Point{0, 0};
//    auto act = [](const auto& r) { r.x  = 1; };

//    error: assignment of member 'Point::x' in read-only object
//    act(p);
}

TEST_CASE("change by 'interface'", "[value reference]")
{
    auto pp = new Point{0, 0};
    auto act = finally([&] { delete pp; });
    IPoint* pq = pp;

    pq->change(1, 1);

    REQUIRE(pp->x == 1);
}

TEST_CASE("create on stack", "[value reference]")
{
    auto i = 0;
    auto p = Point{0, 0};
    auto upi = static_cast<long>(reinterpret_cast<uintptr_t>(&i));
    auto upp = static_cast<long>(reinterpret_cast<uintptr_t>(&p));

    CHECK(abs(upi - upp) < 32);
}

TEST_CASE("create on heap", "[value reference]")
{
    auto i = 0;
    auto pp = new Point{0, 0};
    auto upi = static_cast<long>(reinterpret_cast<uintptr_t>(&i));
    auto upp = static_cast<long>(reinterpret_cast<uintptr_t>(pp));

    CHECK(abs(upi - upp) > 32 * 1024);
}
