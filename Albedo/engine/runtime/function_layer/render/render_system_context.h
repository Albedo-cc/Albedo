#pragma once

#include "render_system_types.h"

namespace Albedo {
namespace Runtime
{

	class RenderSystemContext
	{
		friend class RenderSystem;
	public:
		static constexpr const uint32_t MAX_FRAME_IN_FLIGHT = 2;

	public:
		static FrameState& GetCurrentFrameState() { return m_frame_states[m_current_frame]; }

	private:
		static std::vector<FrameState> m_frame_states;
		static size_t m_current_frame/* = 0*/;
		static std::shared_ptr<RHI::CommandPool	> m_command_pool_resetable;

	private: // Interfaces for RenderSystem
		static void Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context);
		static void SwitchToNextFrame() { m_current_frame = (m_current_frame + 1) % MAX_FRAME_IN_FLIGHT; }
	};

}} // namespace Albedo::Runtime