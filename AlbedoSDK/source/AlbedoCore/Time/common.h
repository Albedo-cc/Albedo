#pragma once

#include <chrono>
#include <thread>

namespace Albedo
{

	template <class _Rep, class _Period>
	void inline Sleep(const std::chrono::duration<_Rep, _Period>& duration)
	{
		std::this_thread::sleep_for(duration);
	}

} // namespace Albedo