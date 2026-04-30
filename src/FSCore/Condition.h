#pragma once

namespace FSC
{

    template<typename T>
    class Condition
    {
        bool Test(T* owner);

    public:
        bool m_IsExpected = true;

        virtual bool OnTest(T* owner) = 0;

        friend Transition<T>;
    };

}

#include "Condition.inl"