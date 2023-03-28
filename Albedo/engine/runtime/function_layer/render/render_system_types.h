#pragma once

#include <AlbedoRHI.hpp>

#include <core/math/math.h>

#include <memory>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	// Type List
	struct FrameState;
	struct UniformBuffer;

	struct FrameState // [MAX_FRAME_IN_FLIGHT]
	{
		std::unique_ptr<RHI::Fence>			m_fence_in_flight;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_image_available;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_render_finished;
		std::shared_ptr<RHI::VMA::Buffer> m_uniform_buffer;
		std::shared_ptr<RHI::CommandBuffer> m_command_buffer; // [Q]: Using Reference caused a bug!
	};

	struct UniformBuffer
	{
		Matrix4f matrix_model;
		Matrix4f matrix_viewing; // Projection * View
	};

}} // namespace Albedo::Runtime