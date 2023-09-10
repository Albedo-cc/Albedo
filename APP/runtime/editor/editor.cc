#include "editor.h"

#include <AlbedoSystem/UI/UI_system.h>

namespace Albedo{
namespace APP
{

	void
	Editor::
	Initialize()
	{
		UISystem::RegisterUIEvent(new UIEvent
		("Main Camera", []()->void
		{
			ImGui::Begin("Main Camera");
			{
				ImGui::Button("Hello World");
			}
			ImGui::End();
		}));
	}

	void
	Editor::
	Destroy()
	{

	}

	void
	Editor::
    Tick()
	{

	}
	
}} // namespace Albedo::APP