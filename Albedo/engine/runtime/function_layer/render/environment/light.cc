#include "light.h"

#include <runtime/function_layer/UI/UI_system.h>

namespace Albedo {
namespace Runtime
{
	
	Light::Light(std::shared_ptr<RHI::VulkanContext> vulkan_context):
		m_light_parameter_buffer{ vulkan_context->m_memory_allocator->
		AllocateBuffer(sizeof(Light::Parameters),
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			true, true, false, true) } // Persistent Memory
	{
		UISystem::instance().RegisterUIEvent(
			"Light Parameters", [this]()
			{
				ImGui::Begin("Light Parameters");

				ImGui::InputFloat("Light Position X", &parameters.light_position[0]);
				ImGui::InputFloat("Light Position Y", &parameters.light_position[1]);
				ImGui::InputFloat("Light Position Z", &parameters.light_position[2]);
				ImGui::Separator();
				ImGui::InputFloat("View Position X", &parameters.view_position[0]);
				ImGui::InputFloat("View Position Y", &parameters.view_position[1]);
				ImGui::InputFloat("View Position Z", &parameters.view_position[2]);

				ImGui::End();
			});
	}
	
}} // namespace Albedo::Runtime