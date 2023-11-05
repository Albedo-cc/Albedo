#include "control_system.h"

namespace Albedo
{
   
	void
	ControlSystem::Process()
	{
		
	}

	static void RegisterControlEvent(ControlEvent* event); // new ControlEvent()
    static void DeleteControlEvent(std::string_view name);

} // namespace Albedo