#pragma once

#include <AlbedoCore/Norm/concepts.h>

namespace Albedo
{
	// Functions
	template<UnsignedInteger USInt>
	bool inline IsPowerOfTwo(USInt number)
	{
		return number && !(number & (number - 1));
	}

} // namespace Albedo