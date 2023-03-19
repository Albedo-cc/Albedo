#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class PresentPipeline  final :
		public RHI::GraphicsPipeline
	{
	public:
		PresentPipeline() = delete;
		PresentPipeline(std::shared_ptr<RHI::VulkanContext> vulkan_context,
									 VkRenderPass owner, uint32_t subpass_bind_point,
									 VkPipeline base_pipeline = VK_NULL_HANDLE, int32_t base_pipeline_index = -1);

	private:
		virtual std::vector<VkPipelineShaderStageCreateInfo>		prepare_shader_stage_state() override;
		virtual VkPipelineLayoutCreateInfo										prepare_pipeline_layout_state()  override;

		virtual VkPipelineVertexInputStateCreateInfo						prepare_vertex_inpute_state() override;
		virtual VkPipelineInputAssemblyStateCreateInfo				prepare_input_assembly_state() override;
		virtual VkPipelineViewportStateCreateInfo							prepare_viewport_state() override;
		virtual VkPipelineRasterizationStateCreateInfo					prepare_rasterization_state() override;
		virtual VkPipelineMultisampleStateCreateInfo					prepare_multisampling_state() override;
		virtual VkPipelineDepthStencilStateCreateInfo					prepare_depth_stencil_state() override;
		virtual VkPipelineColorBlendStateCreateInfo						prepare_color_blend_state() override;
		virtual VkPipelineDynamicStateCreateInfo							prepare_dynamic_state() override;
	};

}} // namespace Albedo::Runtime