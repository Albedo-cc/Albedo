#pragma once

#include <list>
#include <string>
#include <functional>
#include <unordered_map>

namespace Albedo
{

	struct ActionType
	{
		using  Action = const unsigned int;
		static Action Press;	// Just Pressed
		static Action Hold;		//Pressed and Holding
		static Action Detach;	// Just Released  (a special Release action)
		static Action Release;	//	Released
		static inline Action ACTION_TYPE_COUNT = 4;
	};

} // namespace Albedo