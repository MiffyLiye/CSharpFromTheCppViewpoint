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

TEST_CASE("copy assignment", "[value reference]")
{
    auto p = Point{0, 0};
    auto vq = p;
    auto& rq = p;

    p.x = 1;

    REQUIRE(vq.x == 0);
    REQUIRE(rq.x == 1);
}

TEST_CASE("change by 'interface'", "[value reference]")
{
    auto p = Point{0, 0};
    IPoint* ip = &p;

    ip->change(1, 1);

    REQUIRE(p.x == 1);
}

TEST_CASE("pass read only parameter", "[value reference]")
{
    auto p = Point{0, 0};
//    auto act = [](const auto& r) { r.x  = 1; };

//    error: assignment of member 'Point::x' in read-only object
//    act(p);
}

TEST_CASE("create on stack / heap", "[value reference]")
{
    auto i = 0;
    auto vp = Point{0, 0};
    auto rp = new Point{0, 0};
    auto on_scope_exit = finally([&]() { delete rp; });

    auto pi = static_cast<long>(reinterpret_cast<uintptr_t>(&i));
    auto pvp = static_cast<long>(reinterpret_cast<uintptr_t>(&vp));
    auto prp = static_cast<long>(reinterpret_cast<uintptr_t>(rp));

    CHECK(abs(pi - pvp) < 32);
    CHECK(abs(pi - prp) > 32 * 1024);
}
