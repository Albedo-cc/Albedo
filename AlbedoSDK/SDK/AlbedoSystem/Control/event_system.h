#pragma once

#include "control_event.h"

#include <AlbedoCore/Event/event.h>

#include <deque>
#include <string>
#include <functional>

namespace Albedo
{
   
	class EventSystem
	{
	public:
		static void Initialize();
		static void Process();

	public:
		static auto ControlEvent(); // Singleton
		static auto UIEvent();		// Singleton
		 
	private:
		static inline std::deque<Event> m_immediate_events; // Execute in Event Thread
		static inline std::deque<Event> m_deferred_events;
	};

} // namespace Albedo