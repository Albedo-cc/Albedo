#include "basic_event.h"

#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Core/Norm/assert.h>

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
		ALBEDO_ASSERT(m_action);
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
		ALBEDO_ASSERT(!m_name.empty());
		ALBEDO_ASSERT(m_action);
		sm_event_count++;
	}

} // namespace Albedo