#pragma once

#include <limits>

namespace Albedo
{
#ifndef ALBEDO_CORE_LOW_PRECISION_MODE
   	using Precision = double;
#else
    using Precision = float;
#endif
	using FloatType = Precision;

    bool inline FloatEqual(FloatType a, FloatType b)
    {
        return std::abs(a - b) < std::numeric_limits<FloatType>::epsilon();
    }

    bool inline FloatGreater(FloatType a, FloatType b)
    {
        return a - b >= std::numeric_limits<FloatType>::epsilon();
    }

    bool inline FloatLess(FloatType a, FloatType b)
    {
        return b - a >= std::numeric_limits<FloatType>::epsilon();
    }

} // namespace Albedo