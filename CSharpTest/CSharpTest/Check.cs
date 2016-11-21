using Xunit;

namespace CSharpTest
{
    public class Check
    {
        public static void True(bool predicate)
        {
            Assert.True(predicate);
        }
    }
}