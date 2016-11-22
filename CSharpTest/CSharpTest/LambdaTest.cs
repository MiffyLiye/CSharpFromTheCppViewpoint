using System;
using System.Collections.Generic;
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

            act();
            Assert.True(n == "May");

            m = "June";

            act();
            Assert.True(n == "June");
        }

        [Fact]
        public void capture_by_reference_surprise()
        {
            var months = new[] {"May", "June"};
            var n = "";
            var acts = new List<Action>();

            for (var i = 0; i < 1; i++)
            {
                acts.Add(() => { n = months[i]; });
            }
            acts[0]();

            Check.True(n == "June");
        }

        [Fact]
        public void capture_by_value()
        {
            var months = new[] {"May", "June"};
            var n = "";
            var acts = new List<Action>();

            for (var i = 0; i < 1; i++)
            {
                var t = i;
                acts.Add(() => { n = months[t]; });
            }
            acts[0]();

            Assert.True(n == "May");
        }

        [Fact]
        public void breaking_change()
        {
            var months = new[] {"May", "June"};
            var n = "";
            var acts = new List<Action>();

            foreach (var month in months)
            {
                acts.Add(() => { n = month; });
            }
            acts[0]();

            Check.True(n == "May");
        }
    }
}