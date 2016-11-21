using System;
using System.Threading.Tasks;
using Xunit;

namespace CSharpTest
{
    public class LambdaTest
    {
        [Fact]
        public void capture_by_reference()
        {
            var m = "May";
            Action act = () => { m = "June"; };

            act();

            Assert.True(m == "June");
        }

        [Fact]
        public void capture_by_reference_again()
        {
            var m = "May";
            var n = "";
            Action act = () => { n = m; };

            m = "June";
            act();

            Assert.True(n == "June");
        }

        [Fact]
        public void capture_by_reference_surprise()
        {
            var j = -1;
            Task task = null;

            for (var i = 0; i < 1; i++)
            {
                task = Task.Run(() => { j = i; });
            }
            task?.Wait();

            Check.True(j == 1);
        }

        [Fact]
        public void capture_by_value()
        {
            var j = -1;
            Task task = null;

            for (var i = 0; i < 1; i++)
            {
                var t = i;
                task = Task.Run(() => { j = t; });
            }
            task?.Wait();

            Assert.True(j == 0);
        }

        [Fact]
        public void breaking_change()
        {
            var months = new[] {"May", "June"};
            var n = "";
            Task task = null;

            foreach (var month in months)
            {
                task = task ?? Task.Run(() => { n = month; });
            }

            task?.Wait();

            Check.True(n == "May");
        }
    }
}
