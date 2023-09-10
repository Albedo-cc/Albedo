#include "basic_event.h"

#include <AlbedoCore/Log/log.h>

namespace Albedo
{
	
	bool
	BasicEvent::
	Trig()
	{
		return !m_condition || m_condition();
	}

	void BasicEvent::Act()
	{
		assert(m_action);
		m_action();
	}

	BasicEvent::
	BasicEvent(std::string name,
		std::function<void()> action,
		std::function<bool()> condition/* = nullptr*/):
		m_name{std::move(name)},
		m_condition{std::move(condition)},
		m_action{std::move(action)}
	{
		assert(!m_name.empty());
		assert(m_action);
		sm_event_count++;
	}

} // namespace Albedo