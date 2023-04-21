#include "palette.h"

namespace Albedo {
namespace Runtime
{

	void Palette::SetupCameraMatrics(
		std::shared_ptr<RHI::DescriptorSet>	 SET0_ubo,
		std::shared_ptr<RHI::VMA::Buffer> matrics)
	{
		SET0_ubo->WriteBuffer(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			binding_camera_matrics,
			matrics);
	}

	void Palette::SetupLightParameters(
		std::shared_ptr<RHI::DescriptorSet>	 SET0_ubo,
		std::shared_ptr<RHI::VMA::Buffer> light_parameters)
	{
		SET0_ubo->WriteBuffer(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			binding_light_parameters,
			light_parameters);
	}

	void Palette::BindDescriptorSetUBO(
		std::shared_ptr<RHI::CommandBuffer> commandBuffer,
		RHI::GraphicsPipeline* pipeline,
		std::shared_ptr<RHI::DescriptorSet> ubo_set)
	{
		VkDescriptorSet boundSets[1]{ *ubo_set };

		vkCmdBindDescriptorSets(*commandBuffer,
			pipeline->GetPipelineBindPoint(),
			pipeline->GetPipelineLayout(),
			set_uniform_buffers, 1, boundSets,
			0, nullptr);
	}

	void Palette::SetupPBRBaseColor(
		std::shared_ptr<RHI::DescriptorSet>	 SET1_textures,
		std::shared_ptr<RHI::VMA::Image> base_color)
	{
		SET1_textures->WriteImage(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			binding_pbr_base_color, 
			base_color);
	}

	void Palette::BindDescriptorSetMaterial(
		std::shared_ptr<RHI::CommandBuffer> commandBuffer,
		RHI::GraphicsPipeline* pipeline,
		std::shared_ptr<RHI::DescriptorSet> material_set)
	{
		VkDescriptorSet boundSets[1]{ *material_set };

		vkCmdBindDescriptorSets(*commandBuffer,
			pipeline->GetPipelineBindPoint(),
			pipeline->GetPipelineLayout(),
			set_materials, 1, boundSets,
			0, nullptr);
	}

}} // namespace Albedo::Runtime