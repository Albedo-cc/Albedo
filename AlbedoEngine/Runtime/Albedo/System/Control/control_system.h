#pragma once

#include "keyboard.h"
#include "mouse.h"

#include <list>
#include <string>
#include <functional>

namespace Albedo
{
	struct ControlType
	{
		using  Action = unsigned int;
		static Action Press;	// Just Pressed
		static Action Hold;		//Pressed and Holding
		static Action Detach;	// Just Released  (a special Release action)
		static Action Release;	//	Released
		static inline Action ACTION_TYPE_COUNT = 4;
	};

	class ControlSystem final
	{
	public:
		//static void RegisterControlEvent(ControlEvent* event); // new ControlEvent()
        //static void DeleteControlEvent(std::string_view name);

	public:
		static void Initialize();
		static void Process();
		 
	private:
		using EventName = std::string;
		// Keyboard Events
		//using KeyboardEventMap = std::unordered_map<Keyboard::Key, std::list<KeyboardEvent>>;
		//static inline  std::unordered_map<EventName, KeyboardEvent*> m_registry_keyborad_events;
		//inline inline  static std::array<KeyboardEventMap, Action::ACTION_TYPE_COUNT> m_keyboard_events;
		//
		//// Mouse Scroll Events
		//static inline  std::unordered_map<EventName, MouseScrollEvent*> m_registry_mouse_scroll_events;
		//static inline  std::list<MouseScrollEvent> m_mouse_scroll_events;
	};

} // namespace Albedo