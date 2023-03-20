#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoLog.hpp>

namespace Albedo {
namespace Runtime
{

	class RuntimeContext
	{
		friend class RuntimeModule;
		RuntimeContext() = default;
	public:
		bool IsRunning() const { return m_is_running; }
		void Stop(const char* caller_signature)
		{
			m_is_running = false;
			log::critical("{} stoped Albedo Runtime Context!", caller_signature);
		}

	private:
		bool m_is_running = false;
	};

	class RuntimeLayer
	{
	public:
		virtual void Update() = 0;
		RuntimeLayer() = delete;
		RuntimeLayer(RuntimeContext& runtime_context) : m_runtime_context{ runtime_context } {}

	protected:
		RuntimeContext& m_runtime_context;
	};

}} // namespace Albedo::Runtime