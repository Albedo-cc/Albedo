#pragma once

#include <AlbedoRHI.hpp>

#include <core/math/math.h>

#include <memory>

namespace Albedo {
namespace Runtime
{
	// Type List
	struct FrameState;
	struct UniformBuffer;

	constexpr const uint32_t MAX_FRAME_IN_FLIGHT = 2;
	struct FrameState // [MAX_FRAME_IN_FLIGHT]
	{
		std::unique_ptr<RHI::Fence>			m_fence_in_flight;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_image_available;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_render_finished;
		std::shared_ptr<RHI::VMA::Buffer> m_uniform_buffer;
		std::shared_ptr<RHI::CommandBuffer> m_command_buffer; // [Q]: Using Reference caused a bug!

		static uint32_t	GetCurrentFrame(bool increase = false)
		{
			static uint32_t current_frame{ 0 };
			if (increase) current_frame = (current_frame + 1) % MAX_FRAME_IN_FLIGHT;
			return current_frame;
		}
	};

	struct UniformBuffer
	{
		Matrix4f matrix_model;
		Matrix4f matrix_view;
		Matrix4f matrix_projection;
	};

}} // namespace Albedo::Runtime