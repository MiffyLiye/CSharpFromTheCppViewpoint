using System;
using Xunit;

namespace CSharpTest
{
    public interface IPoint
    {
        void Change(int x, int y);
    }

    public struct VPoint : IPoint
    {
        public int X { get; set; }
        public int Y { get; set; }

        public void Change(int x, int y)
        {
            X = x;
            Y = y;
        }
    }

    public class ValueTypeTest
    {
        [Fact]
        public void copy_by_value()
        {
            var p = new VPoint {X = 0, Y = 0};
            var q = p;

            p.X = 1;

            Assert.True(q.X == 0);
        }

        [Fact]
        public void change_by_interface()
        {
            var p = new VPoint {X = 0, Y = 0};

            p.Change(1, 2);

            Assert.True(p.X == 1);
        }

        [Fact]
        public void boxing_then_change_by_interface()
        {
            var p = new VPoint {X = 0, Y = 0};

            ((IPoint) p).Change(1, 2);

            Assert.True(p.X == 0);
        }

        [Fact]
        public void created_on_stack()
        {
            var i = 0;
            var p = new VPoint {X = 0, Y = 0};
            unsafe
            {
                var pi = &i;
                var pp = &p;

                Check.True(Math.Abs((long)pi - (long)pp) < 32);
            }
        }
    }
}
