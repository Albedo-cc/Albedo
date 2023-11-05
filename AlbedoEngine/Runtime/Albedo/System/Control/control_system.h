#pragma once

#include "control_event.h"

#include <deque>
#include <string>
#include <functional>

#include <Albedo/Core/Event/event_manager.h>

namespace Albedo
{
	using ControlEvent = BasicEvent;
	class ControlSystem final
	{
	public:
		static void RegisterControlEvent(ControlEvent* event); // new ControlEvent()
        static void DeleteControlEvent(std::string_view name);

	public:
		static void Initialize();
		static void Process();
		 
	private:
		static inline EventManager m_control_events;
	};

} // namespace Albedo