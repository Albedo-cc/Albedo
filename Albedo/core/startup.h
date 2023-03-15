#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoTime.hpp>
#include <AlbedoLog.hpp>

#include "runtime/runtime.h"

namespace Albedo
{
	class APP :
		public pattern::Singleton<APP>
	{
		friend class pattern::Singleton<APP>;

		APP()
		{
			m_stopwatch.start();

			log::info("Initialization cost {} ms", m_stopwatch.split(false).milliseconds());
		}

		~APP()
		{
			log::info("Albedo is terminating, total run time {} s", m_stopwatch.split().seconds());
		}

	public:
		void run()
		{
			Runtime::RenderSystem rs{};
		}
	private:
		time::StopWatch<uint64_t> m_stopwatch;
	};
}