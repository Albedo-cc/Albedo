#pragma once

#include "action_types.h"

namespace Albedo
{

	class Keyboard
	{
		friend class ControlSystem;
	public:
		using Event = std::function<void()>;
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
		using KeyboardEventRegistry = std::unordered_map<std::string, Keyboard::Event*>;
		static inline KeyboardEventRegistry sm_keyborad_event_registry;
		using KeyboardEventMap = std::unordered_map<Keyboard::Key, std::list<Keyboard::Event>>;
		static inline KeyboardEventMap sm_keyboard_events[ActionType::ACTION_TYPE_COUNT];

	private:
		Keyboard() = delete;
	};

	struct KeyboardEventCreateInfo
	{
		std::string				name;
		Keyboard::Key			key;
		ActionType::Action		action;
		Keyboard::Event			callback;
	};

} // namespace Albedo