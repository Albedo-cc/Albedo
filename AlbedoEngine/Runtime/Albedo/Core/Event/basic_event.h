#pragma once

#include <string>
#include <string_view>
#include <functional>

namespace Albedo
{

	class BasicEvent
	{
	public:
		auto Trig() -> bool;// if   Trig()
		void Act()		   ;// then Act();

		auto GetName() const -> std::string_view { return m_name; }
		static auto GetTotalEventCount() -> uint64_t { return sm_event_count; }
		
	public:
		BasicEvent() = delete;
		BasicEvent(std::string name,
			std::function<void()> action,
			std::function<bool()> condition = nullptr); // nullptr == Unconditional
		virtual ~BasicEvent() { sm_event_count--; }

	private:
		std::string m_name;
		std::function<void()> m_action;
		std::function<bool()> m_condition;
		
	private:
		static inline uint64_t sm_event_count = 0;
	};

	
} // namespace Albedo