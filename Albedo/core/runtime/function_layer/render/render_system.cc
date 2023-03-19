#include "render_system.h"

#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{

	RenderSystem::RenderSystem() :
		m_window_system{ std::make_shared<WindowSystem>() },
		m_vulkan_context{ std::make_shared<RHI::VulkanContext>(m_window_system->GetWindow()) }
	{
		m_render_passes.emplace_back(std::make_unique<ForwardRenderPass>(m_vulkan_context));
	}

}} // namespace Albedo::Runtime