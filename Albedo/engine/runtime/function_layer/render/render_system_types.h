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
		std::shared_ptr<RHI::CommandBuffer> m_command_buffer;
		// Global Descriptor Set
		std::shared_ptr<RHI::DescriptorSet> m_global_descriptor_set;
		std::shared_ptr<RHI::VMA::Buffer> m_uniform_buffer;
	};

	struct UniformBuffer
	{
		glm::mat4x4 matrix_model;
		glm::mat4x4 matrix_view;
		glm::mat4x4 matrix_projection;
		//alignas(16) Matrix4f matrix_model;
		//alignas(16) Matrix4f matrix_view;
		//alignas(16) Matrix4f matrix_projection;
	};

}} // namespace Albedo::Runtime