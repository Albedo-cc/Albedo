#pragma once

#include "input/keyboard.h"
#include "input/mouse.h"

#include <functional>

namespace Albedo {
namespace Runtime
{

	namespace Action
	{
		enum Type
		{
			Press = GLFW_PRESS, Hold = GLFW_REPEAT, Release = GLFW_RELEASE
		};
	}

	struct KeyboardEvent
	{
		Keyboard::Key				key;
		Action::Type					action;
		std::function<void()>	function;
	};

	struct MouseButtonEvent
	{
		Mouse::Button				button;
		Action::Type					action;
		std::function<void()>	function;
	};

}} // namespace Albedo::Runtime