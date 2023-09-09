#pragma once

#include "basic_event.h"

#include <list>
#include <unordered_map>

namespace Albedo
{
	class EventManager
	{
		using EventList     = std::list<BasicEvent*>;
        using EventRegistry = std::unordered_map<std::string_view, BasicEvent**>;
	public:
		void Register(BasicEvent* event);
		void Delete(std::string_view name);
		void Process(); // Do "If event->Trig() then event->Act()" for every event.
		void TrigAll(); // Execute all events without its condition. (For Performance)
		void Clear(std::string_view signature);

		auto GetSize() const -> size_t { return m_events.size(); }

	private:
		EventList       m_events;
        EventRegistry   m_registry;

	public:
		EventManager() = default;
		~EventManager();
	};
	
} // namespace Albedo