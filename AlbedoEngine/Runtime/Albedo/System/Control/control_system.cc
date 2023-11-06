#include "control_system.h"
#include <GLFW/glfw3.h>

namespace Albedo
{
	ControlType::Action Release	= GLFW_RELEASE;	// Released
	ControlType::Action Press	= GLFW_PRESS;	// Just Pressed
	ControlType::Action Hold	= GLFW_REPEAT;	// Pressed and Holding
	ControlType::Action Detach	= Hold + 1;		// Just Released  (a special Release action)

	void
	ControlSystem::Process()
	{
		
	}

} // namespace Albedo