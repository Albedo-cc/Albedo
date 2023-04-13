#include "palette.h"

namespace Albedo {
namespace Runtime
{

	void Palette::SetupCameraMatrics(std::shared_ptr<RHI::VMA::Buffer> matrics)
	{
		SET0_ubo->WriteBuffer(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			binding_camera_matrics,
			matrics);
	}

	void Palette::SetupLightParameters(std::shared_ptr<RHI::VMA::Buffer> light_parameters)
	{
		SET0_ubo->WriteBuffer(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			binding_light_parameters,
			light_parameters);
	}

	void Palette::SetupPBRBaseColor(std::shared_ptr<RHI::VMA::Image> base_color)
	{
		SET1_texture->WriteImage(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			binding_pbr_base_color, 
			base_color);
	}

	void Palette::initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::DescriptorPool> descriptorPool)
	{
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
				.binding = binding_light_parameters,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1, // Not Array
				.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			} });

		SET1_texture = descriptorPool->AllocateDescriptorSet({
			VkDescriptorSetLayoutBinding
			{
				.binding = binding_pbr_base_color,
				.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
				.descriptorCount = 1,
				.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
			} });
	}
	
}} // namespace Albedo::Runtime