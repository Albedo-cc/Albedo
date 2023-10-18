#include "dearimgui.h"

#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>

#include <AlbedoEditor/editor.h>

namespace Albedo
{

	void
	ImGuiPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{	
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void
	ImGuiPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		ImGui::Render(); // Prepare the data for rendering so you can call GetDrawData()
		// Note: at this point both vkCmdSetViewport() and vkCmdSetScissor() have been called.
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), *commandbuffer);
		ImGui::UpdatePlatformWindows();
	}

} // namespace Albedo