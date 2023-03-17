#pragma once

#include "../graphics_pipeline.h"

namespace Albedo {
namespace Runtime
{

	class FrontRenderPipeline:
		public GraphicsPipeline
	{
	public:
		virtual void render() final override {};

	public:
		FrontRenderPipeline() = delete;
		FrontRenderPipeline(std::shared_ptr<RHI::VulkanManager> vulkan_manager);

	private:
		virtual VkPipelineShaderStageCreateInfo prepare_vertex_shader_stage(std::string_view vertex_shader) final override;
		virtual VkPipelineShaderStageCreateInfo prepare_fragment_shader_stage(std::string_view fragment_shader) final override;

		virtual VkPipelineVertexInputStateCreateInfo prepare_vertex_inpute_state() final override;
		virtual VkPipelineInputAssemblyStateCreateInfo prepare_input_assembly_state() final override;
		virtual VkPipelineViewportStateCreateInfo prepare_viewport_state() final override;
		virtual VkPipelineRasterizationStateCreateInfo prepare_rasterization_state() final override;
		virtual VkPipelineMultisampleStateCreateInfo prepare_multisampling_state() final override;
		// prepare_depth_and_stencil_state();
		virtual VkPipelineColorBlendStateCreateInfo prepare_color_blend_state(bool is_mix_mode = true) final override;
		virtual VkPipelineDynamicStateCreateInfo prepare_dynamic_state() final override;
	};

}} // namespace Albedo::Runtime