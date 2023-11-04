#include "event_manager.h"

#include <Albedo/Core/Log/log.h>

namespace Albedo
{

	void
	EventManager::
	Register(BasicEvent* event)
	{
		auto event_name = event->GetName();
		auto newbee = m_registry.find(event->GetName());
		if (newbee == m_registry.end())
		{
			Log::Debug("Albedo Core BasicEvent: Registering a new event {}", event_name);
			m_registry[event_name] = &m_events.emplace_back(event);
		}
		else Log::Warn("Albedo Core BasicEvent: Failed to register the event {}", event_name);
	}

	void
	EventManager::
	Delete(std::string_view name)
	{
		auto exiler = m_registry.find(name);
		if (exiler != m_registry.end())
		{
			Log::Debug("Albedo Core BasicEvent: Deleting an event {}", name);
			(*exiler->second) = nullptr;
			m_registry.erase(exiler);
		}
		else Log::Warn("Albedo Core BasicEvent: Failed to delete the event {}", name);
	}

	void
	EventManager::
	Process()
	{
		for (auto event_iter = m_events.begin();
				  event_iter != m_events.end();)
		{
			auto event = *event_iter;
			if (event)
			{
				if (event->Trig())
					event->Act();
				++event_iter;
			}
			else m_events.erase(event_iter++);
		}
	}

	void
	EventManager::
	TrigAll()
	{
		for (auto& event : m_events)
		{
			if (event) event->Act();
		}
	}

	void
	EventManager::
	Clear(std::string_view signature)
	{
		Log::Warn("{} is clearing BasicEvent Manager({})!", signature, uintptr_t(this));

		for (auto& event : m_events) delete event;
		m_events.clear();

		m_registry.clear();
	}

	EventManager::
	~EventManager()
	{
		for (auto& event : m_events)
		{
			delete event;
			event = nullptr;
		}
	}
	
} // namespace Albedo