#pragma once

#include <chrono>
#include <thread>

namespace Albedo
{
	using namespace std::chrono_literals;
   
	template <class _Rep, class _Period>
	inline void Sleep(const std::chrono::duration<_Rep, _Period>& duration)
	{
		std::this_thread::sleep_for(duration);
	}

} // namespace Albedo