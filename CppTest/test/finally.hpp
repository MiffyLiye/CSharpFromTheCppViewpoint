#pragma once

namespace tcpppl
{
    template <typename F>
    struct FinalAction
    {
        FinalAction(F f) : clean{f}
        {
        }

        ~FinalAction()
        {
            clean();
        }

        F clean;
    };

    template <typename F>
    FinalAction<F> finally(F f)
    {
        return FinalAction<F>(f);
    }
}
