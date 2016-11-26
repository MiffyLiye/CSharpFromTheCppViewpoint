using System;
using Xunit;

namespace CSharpTest
{
    public interface IPoint
    {
        int X { get; }
        int Y { get; }
        void Change(int x, int y);
    }

    public class RPoint : IPoint
    {
        public int X { get; set; }
        public int Y { get; set; }

        public void Change(int x, int y)
        {
            X = x;
            Y = y;
        }
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

    public class ValueReferenceTypeTest
    {
        [Fact]
        public void copy_assignment()
        {
            var rp = new RPoint {X = 0, Y = 0};
            var vp = new VPoint {X = 0, Y = 0};
            var rq = rp;
            var vq = vp;

            rp.X = 1;
            vp.X = 1;

            Assert.True(rq.X == 1);
            Assert.True(vq.X == 0);
        }

        [Fact]
        public void pass_parameter()
        {
            var rp = new RPoint {X = 0, Y = 0};
            var vp = new VPoint {X = 0, Y = 0};
            Action<RPoint> ract = (v) => { v.X = 1; };
            Action<VPoint> vact = (v) => { v.X = 1; };

            ract(rp);
            vact(vp);

            Assert.True(rp.X == 1);
            Assert.True(vp.X == 0);
        }

        [Fact]
        public void change_with_interface_method()
        {
            var rp = new RPoint {X = 0, Y = 0};
            var vp = new VPoint {X = 0, Y = 0};

            rp.Change(1, 2);
            vp.Change(1, 2);

            Assert.True(rp.X == 1);
            Assert.True(vp.X == 1);
        }

        [Fact]
        public void boxing_with_interface()
        {
            var rp = new RPoint {X = 0, Y = 0};
            var vp = new VPoint {X = 0, Y = 0};

            ((IPoint) rp).Change(1, 2);
            ((IPoint) vp).Change(1, 2);

            Assert.True(rp.X == 1);
            Assert.True(vp.X == 0);
        }

        [Fact]
        public void pass_readonly_parameter_by_value()
        {
            var rp = new RPoint {X = 0, Y = 0};

//            The property IPoint.X has no setter
//            Action<IPoint> act = (v) => { v.X = 1; };

//            act(rp);
        }
    }
}