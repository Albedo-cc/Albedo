#pragma once

#include "action_types.h"

namespace Albedo
{

	class Mouse
	{
		friend class ControlSystem;
	public:
		using Button = unsigned int;
		static Button Left, Middle, Right;

	private:
		Mouse() = delete;
	};

	struct MouseButtonEventCreateInfo
	{
		std::string				name;
		Mouse::Button			button;
		ActionType::Action		action;
		std::function<void()>	callback;
	};

	struct MouseScrollEventCreateInfo
	{
		std::string	name;
		std::function<void(double x, double y)>	callback;
	};

} // namespace Albedo