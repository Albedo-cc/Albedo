#pragma once

#include <Albedo/Core/Norm/concepts.h>
#include <limits>
#include <cmath>

namespace Albedo
{
    template<FloatingPoint T>
    bool inline FloatEqual(T a, T b)
    {
        return std::abs(a - b) < std::numeric_limits<T>::epsilon();
    }

    template<FloatingPoint T>
    bool inline FloatGreater(T a, T b)
    {
        return a - b >= std::numeric_limits<T>::epsilon();
    }

    template<FloatingPoint T>
    bool inline FloatLess(T a, T b)
    {
        return b - a >= std::numeric_limits<T>::epsilon();
    }

} // namespace Albedo