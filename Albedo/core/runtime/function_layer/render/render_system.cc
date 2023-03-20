#include "render_system.h"

#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{

	RenderSystem::RenderSystem(std::weak_ptr<WindowSystem> window_system) :
		m_vulkan_context{ std::make_shared<RHI::VulkanContext>(window_system.lock()->GetWindow()) },
		m_window_system{ std::move(window_system) },
		// Command Pool
		m_command_pool_reset{ m_vulkan_context,
				m_vulkan_context->m_device_queue_graphics.value(),
				VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT },
		// Framebuffer Pool
		m_framebuffer_pool{ m_vulkan_context },
		// Synchronization
		m_semaphore_image_available{ m_vulkan_context , 0x0 },
		m_semaphore_render_finished{ m_vulkan_context , 0x0 },
		m_fence_in_flight{ m_vulkan_context , VK_FENCE_CREATE_SIGNALED_BIT }
	{
		// Render Passes
		create_render_passes();
		// Framebuffers
		create_framebuffer_pool();
	}

	void RenderSystem::wait_for_next_image_index()
	{
		auto res = m_vulkan_context->NextSwapChainImageIndex(
			m_semaphore_image_available, VK_NULL_HANDLE);

		//if (m_window_system.lock()->IsResized(true) ||
		//	res == VK_ERROR_OUT_OF_DATE_KHR ||
		//	res == VK_SUBOPTIMAL_KHR)
		//{
		//	log::warn("Window Resized!");
		//	m_vulkan_context->RecreateSwapChain();
		//	create_framebuffer_pool(); // Recreate Framebuffers
		//	create_render_passes();		 // Recreate Render Passes
		//}
	}

	void RenderSystem::create_framebuffer_pool()
	{
		m_framebuffer_pool.clear();

		for (size_t i = 0; i < m_vulkan_context->m_swapchain_imageviews.size(); ++i)
		{
			std::vector<VkImageView> attachments{ m_vulkan_context->m_swapchain_imageviews[i] };

			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = *(m_render_passes[render_pass_forward]),
				.attachmentCount = static_cast<uint32_t>(attachments.size()),
				.pAttachments = attachments.data(),
				.width = m_vulkan_context->m_swapchain_current_extent.width,
				.height = m_vulkan_context->m_swapchain_current_extent.height,
				.layers = 1
			};
			m_framebuffer_pool.AllocateFramebuffer(framebufferCreateInfo);
		}
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_unique<ForwardRenderPass>(m_vulkan_context);
	}

}} // namespace Albedo::Runtime