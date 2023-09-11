#pragma once
#include <concepts>

namespace Albedo
{
	// Concepts
	template<typename USInt>
	concept UnsignedInteger = std::unsigned_integral<USInt>;

	template<typename SInt>
	concept SignedInteger = std::integral<SInt>;

	// Functions
	template<UnsignedInteger USInt>
	bool inline IsPowerOfTwo(USInt number)
	{
		return number && !(number & (number - 1));
	}

} // namespace Albedo