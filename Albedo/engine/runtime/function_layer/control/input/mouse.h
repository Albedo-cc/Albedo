#pragma once

#include <GLFW/glfw3.h>

namespace Albedo {
namespace Runtime
{
	
	namespace Mouse
	{
		enum Button
		{
			// Common
			Left			=		GLFW_MOUSE_BUTTON_LEFT,
			Middle	=		GLFW_MOUSE_BUTTON_MIDDLE,
			Right		=		GLFW_MOUSE_BUTTON_RIGHT
		};
	} // namespace Albedo::Runtime::Mouse

}} // namespace Albedo::Runtime