#include "sandbox.h"

#include <Albedo.Core.Log>
#include <Albedo.Editor>

namespace Albedo{
namespace APP
{
	
	void Sandbox()
	{
		// Write your codes here.
		static bool init = false;
		if (!init) Editor::RegisterUIEvent(new UIEvent{
			"Sandbox", []()
			{
				ImGui::Begin("Hello");
				{
					if (ImGui::Button("Click me!"))
					{
						Log::Info("Clicked!");
					}
				}
				ImGui::End();
			} });

		init = true;
	}

}} // namespace Albedo::APP