#pragma once

namespace Albedo
{
	
	using Byte		=	char;
	using UByte		=	unsigned char;
	using EnumBits	=	unsigned int;

	inline void
	MakeOffset(void** start, UByte offset)
	{
		*start = reinterpret_cast<UByte*>(*start) + offset;
	}
	
} // namespace Albedo