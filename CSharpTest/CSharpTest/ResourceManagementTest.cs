using System;
using Xunit;

namespace CSharpTest
{
    public class Holder
    {
        public int Count { get; set; }
    }

    public class Resource : IDisposable
    {
        private Holder Holder { get; set; }

        public Resource(Holder holder)
        {
            Holder = holder;
            holder.Count += 1;
        }

        ~Resource()
        {
            Dispose();
        }

        public void Dispose()
        {
            Holder.Count -= 1;
        }
    }

    public class ResourceManagementTest
    {
        [Fact]
        public void dispose_at_end_of_scope()
        {
            var holder = new Holder();
            using (var r = new Resource(holder))
            {
                Assert.True(holder.Count == 1);
            }

            Assert.True(holder.Count == 0);
        }

        [Fact]
        public void non_deterministic_gc()
        {
            var holder = new Holder();
            Action act = () => { var r = new Resource(holder); };

            act();

            Assert.True(holder.Count == 1);
        }
    }
}
