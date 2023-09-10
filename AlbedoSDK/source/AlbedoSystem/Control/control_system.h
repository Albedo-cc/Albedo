#pragma once

#include "control_event.h"

#include <deque>
#include <string>
#include <functional>

#include <AlbedoCore/Event/basic_event.h>

namespace Albedo
{
   
	class ControlSystem
	{
	public:
		static void Initialize();
		static void Process();

	public:
		static auto ControlEvent(); // Singleton
		static auto UIEvent();		// Singleton
		 
	private:
		static inline std::deque<BasicEvent> m_immediate_events; // Execute in Event Thread
		static inline std::deque<BasicEvent> m_deferred_events;
	};

} // namespace Albedo