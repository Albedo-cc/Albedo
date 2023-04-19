#include "easel.h"

namespace Albedo {
namespace Runtime
{

	Easel::Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		m_vulkan_context{ std::move(vulkan_context) }
	{

		// Descriptor Pool
		m_descriptor_pool	= m_vulkan_context->CreateDescriptorPool(std::vector<VkDescriptorPoolSize>
		{
			{ // Uniform Buffers
				.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = MAX_DESCRIPTOR_POOL_SIZE_UBO,
			},
			{ // Image Samplers
				.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = MAX_DESCRIPTOR_POOL_SIZE_TEXTURE 
			}
		}, MAX_DESCRIPTOR_POOL_SET_COUNT);

		// Canvases
		m_canvases.resize(MAX_CANVAS_COUNT);
		for (size_t i = 0; i < MAX_CANVAS_COUNT; ++i)
		{
			auto& canvas = m_canvases[i];
			canvas.cmd_buffer_front = m_vulkan_context->CreateResetableCommandBuffer(m_vulkan_context->m_device_queue_family_graphics);
			canvas.cmd_buffer_ui = m_vulkan_context->CreateResetableCommandBuffer(m_vulkan_context->m_device_queue_family_graphics);
			// Descriptor Sets
			canvas.palette.initialize(m_vulkan_context, m_descriptor_pool);
			// Sync Meta
			canvas.syncmeta.fence_in_flight = m_vulkan_context->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
			canvas.syncmeta.semaphore_image_available = m_vulkan_context->CreateSemaphore(0x0);
			canvas.syncmeta.semaphore_render_finished = m_vulkan_context->CreateSemaphore(0x0);

			canvas.syncmeta.semaphore_screenshot_finished = m_vulkan_context->CreateSemaphore(0x0);
			canvas.syncmeta.semaphore_ui_finished = m_vulkan_context->CreateSemaphore(0x0);
		}
	}

	Canvas& Easel::WaitCanvas() throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];
		// Sync
		canvas.syncmeta.fence_in_flight->Wait();
		m_vulkan_context->NextSwapChainImageIndex(*canvas.syncmeta.semaphore_image_available, VK_NULL_HANDLE);
		canvas.syncmeta.fence_in_flight->Reset();

		return canvas;
	}

	void Easel::PresentCanvas(std::vector<VkSemaphore> wait_semaphores, bool switch_canvas/* = true*/) throw (RHI::VulkanContext::swapchain_error)
	{
		auto& canvas = m_canvases[m_current_canvas];

		m_vulkan_context->PresentSwapChain(wait_semaphores);

		m_current_canvas = (m_current_canvas + switch_canvas) % MAX_CANVAS_COUNT;
	}

}} // namespace Albedo::Runtime