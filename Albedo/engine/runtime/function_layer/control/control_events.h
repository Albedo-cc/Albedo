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
			Press = GLFW_PRESS,				// Just Pressed
			Hold = GLFW_REPEAT,			// Pressed and Holding
			Detach = Hold + 1,					// Just Released  (a special Release action)
			Release = GLFW_RELEASE,	//	Released
			ACTION_TYPE_COUNT = 4
		};
	}

	using EventName = std::string;
	using ControlEvent = std::function<void()>;

	struct KeyboardEventCreateInfo
	{
		EventName					name;
		Keyboard::Key				key;
		//Keyboard::Key			key2;	// TODO: main key + vice key = combinated action
		Action::Type					action;
		ControlEvent					event;
	};

	struct MouseButtonEventCreateInfo
	{
		EventName					name;
		Mouse::Button				button;
		Action::Type					action;
		ControlEvent					event;
	};

}} // namespace Albedo::Runtime