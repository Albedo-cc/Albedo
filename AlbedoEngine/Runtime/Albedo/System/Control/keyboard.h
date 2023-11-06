#pragma once

namespace Albedo
{

	class Keyboard
	{
	public:
		using Key = unsigned int;
		static Key
		// Alphabat
		A,	B,	C,	D,	E,	F,	G,	H,	I,	J,	K,	L,	M,
		N,	O,	P,	Q,	R,	S,	T,	U,	V,	W,	X,	Y,	Z,
		// Numbers
		Num0,	Num1,	Num2,	Num3,	Num4,
		Num5,	Num6,	Num7,	Num8,	Num9,
		// FX
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
		//Others
		Space, LShift, RShift, ESC, Enter, Tab;

	private:
		Keyboard() = delete;
	};

} // namespace Albedo