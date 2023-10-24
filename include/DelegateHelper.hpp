#pragma once

#include "System/Action_2.hpp"
#include "System/Action_1.hpp"
#include "System/Action.hpp"

#include "custom-types/shared/delegate.hpp"

namespace DelegateHelper
{

    inline System::Action *CreateDelegate(std::function<void()> func)
    {
        return custom_types::MakeDelegate<System::Action*>(func);
    }

    template <typename T1>
    inline System::Action_1<T1> *CreateDelegate(std::function<void(T1)> func)
    {
        return custom_types::MakeDelegate<System::Action_1<T1>*>(func);
    }

    template <typename T1, typename T2>
    inline System::Action_2<T1, T2> *CreateDelegate(std::function<void(T1, T2)> func)
    {
        return custom_types::MakeDelegate<System::Action_2<T1, T2>*>(func);
    }
}