#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoTime.hpp>
#include <AlbedoLog.hpp>

#include "runtime/runtime.h"
#include "net/net.h"

namespace Albedo
{
	class APP :
		public pattern::Singleton<APP>
	{
		friend class pattern::Singleton<APP>;

		APP()
		{
			log::info("Albedo is being initialized");
			m_stopwatch.start();
		}

		~APP()
		{
			log::info("Albedo is being terminated, total run time {} s", m_stopwatch.split().seconds());
		}

	public:
		void Run(int argc, char* argv[])
		{
			Net::NetModule::instance().Run("127.0.0.1", 5200);
			Runtime::RuntimeModule::instance().Run();
		}
	private:
		time::StopWatch<uint64_t> m_stopwatch;
	};
}