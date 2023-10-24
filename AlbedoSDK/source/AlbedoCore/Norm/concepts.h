#pragma once

#include <concepts>

namespace Albedo
{
	
	// Basic Concepts
	template <typename T>
	concept HashableType = requires(T type) {{ std::hash<T>{}(type) } -> std::same_as<std::size_t>;};
	
	// Numerical Concepts
	template<typename T>
	concept UnsignedInteger = std::unsigned_integral<T>;

	template<typename T>
	concept SignedInteger = std::integral<T>;

	template<typename T>
    concept FloatingPoint = std::floating_point<T>;

    template<typename T>
    concept RealNumber = std::integral<T> || std::floating_point<T>;

} // namespace Albedo