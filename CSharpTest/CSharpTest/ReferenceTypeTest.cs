﻿using Xunit;

namespace CSharpTest
{
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

    public class ReferenceTypeTest
    {
        [Fact]
        public void copy_by_reference()
        {
            var p = new RPoint {X = 0, Y = 0};
            var q = p;

            p.X = 1;

            Assert.True(q.X == 1);
        }

        [Fact]
        public void change_by_interface()
        {
            var p = new RPoint {X = 0, Y = 0};

            ((IPoint) p).Change(1, 1);

            Assert.True(p.X == 1);
        }
    }
}