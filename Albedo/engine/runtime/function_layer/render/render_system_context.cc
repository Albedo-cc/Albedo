#include "render_system_context.h"

namespace Albedo {
namespace Runtime
{
	// Initialize Static Variables
	std::vector<FrameState> RenderSystemContext::m_frame_states{};
	size_t RenderSystemContext::m_current_frame = 0;
	std::shared_ptr<RHI::CommandPool	> RenderSystemContext::m_command_pool_resetable{};

	void RenderSystemContext::Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context)
	{
		// Command Pools
		m_command_pool_resetable = vulkan_context->CreateCommandPool(
																vulkan_context->m_device_queue_graphics.value(),
																VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

		// Frame States
		m_frame_states.reserve(MAX_FRAME_IN_FLIGHT);
		for (int i = 0; i < MAX_FRAME_IN_FLIGHT; ++i)
		{
			auto& state = m_frame_states.emplace_back();
			state.m_fence_in_flight = vulkan_context->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
			state.m_semaphore_image_available = vulkan_context->CreateSemaphore(0x0);
			state.m_semaphore_render_finished = vulkan_context->CreateSemaphore(0x0);
			state.m_command_buffer = m_command_pool_resetable->AllocateCommandBuffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY);
			state.m_uniform_buffer = vulkan_context->m_memory_allocator->AllocateBuffer
			(sizeof(UniformBuffer), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, true, false, true); // Persistent Memory
		}
	}

}} // namespace Albedo::Runtime