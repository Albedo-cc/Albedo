#pragma once

#include <AlbedoRHI.hpp>

#include <core/math/math.h>

namespace Albedo {
namespace Runtime
{

	class Palette
	{
		friend class Canvas;
	public:
		enum Layout // Same order as shaders
		{
			MAX_SET_COUNT = 2,
			// layout(set = 0) Descriptor Set UBO
			set_uniform_buffers							=	0,
				binding_camera_matrics					=	0,
				binding_pbr_parameters					=	1,
				MAX_UNIFORM_BUFFER_COUNT	=	1+1,

			// layout(set = 1) Textures
			set_textures											=	1,
				binding_base_color							=	0,
				MAX_TEXTURE_COUNT					=	1,
		};

	public: // Interfaces
		struct UBO_Camera_Matrics
		{
			glm::mat4x4 matrix_model;
			glm::mat4x4 matrix_view;
			glm::mat4x4 matrix_projection;
		};
		static UBO_Camera_Matrics& GetCameraMatrics() { static UBO_Camera_Matrics mat; return mat; }

	public:
		void Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::DescriptorPool> descriptorPool)
		{
			// Allocate Buffers
			S0B0_camera_matrics = vulkan_context->m_memory_allocator->AllocateBuffer
			(sizeof(UBO_Camera_Matrics), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, true, true, false, true); // Persistent Memory

			// Allocate Descriptor Sets
			SET0_ubo = descriptorPool->AllocateDescriptorSet({
				VkDescriptorSetLayoutBinding
				{
					.binding = binding_camera_matrics,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					.descriptorCount = 1, // Not Array
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
				},
				VkDescriptorSetLayoutBinding
				{
					.binding = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
				}
			});
		}

	private: // Hide Descriptor Sets and Memory Objects to users and bind by Canvas automatically
		std::shared_ptr<RHI::DescriptorSet> SET0_ubo;
		std::shared_ptr<RHI::VMA::Buffer> S0B0_camera_matrics;
		std::shared_ptr<RHI::VMA::Buffer> S0B1_pbr_parameters;
	};

}} // namespace Albedo::Runtime