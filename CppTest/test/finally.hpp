#pragma once

template<typename F>
struct Final_action
{
    Final_action(F f) : clean{f} {}

    ~Final_action() { clean(); }

    F clean;
};

template<typename F>
Final_action<F> finally(F f)
{
    return Final_action<F>(f);
}
