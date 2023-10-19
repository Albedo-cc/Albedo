#pragma once
#include <concepts>

namespace Albedo
{

	template<typename T>
	concept UnsignedInteger = std::unsigned_integral<T>;

	template<typename T>
	concept SignedInteger = std::integral<T>;

    template<typename T>
    concept RealNumber = std::integral<T> || std::floating_point<T>;

} // namespace Albedo