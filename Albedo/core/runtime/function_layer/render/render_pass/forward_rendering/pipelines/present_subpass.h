//#pragma once
//
//#include "../graphics_pipeline.h"
//
//namespace Albedo {
//namespace Runtime
//{
//
//	class FrontRenderPipeline:
//		public GraphicsPipeline
//	{
//	public:
//		virtual void render() final override {};
//
//	public:
//		FrontRenderPipeline() = delete;
//		FrontRenderPipeline(std::shared_ptr<RHI::VulkanManager> vulkan_manager);
//
//	private:
//		virtual std::string set_vertex_shader() final override { return "resource/shader/default.vert.spv"; }
//		virtual std::string set_fragment_shader() final override { return "resource/shader/default.frag.spv"; }
//
//		virtual VkPipelineLayoutCreateInfo prepare_pipeline_layout_state() override;
//		virtual VkRenderPassCreateInfo prepare_render_pass_state() override;
//
//		virtual VkPipelineVertexInputStateCreateInfo prepare_vertex_inpute_state() override;
//		virtual VkPipelineInputAssemblyStateCreateInfo prepare_input_assembly_state() override;
//		virtual VkPipelineViewportStateCreateInfo prepare_viewport_state() override;
//		virtual VkPipelineRasterizationStateCreateInfo prepare_rasterization_state() override;
//		virtual VkPipelineMultisampleStateCreateInfo prepare_multisampling_state() override;
//		virtual VkPipelineDepthStencilStateCreateInfo prepare_depth_stencil_state() override;
//		virtual VkPipelineColorBlendStateCreateInfo prepare_color_blend_state(bool is_mix_mode = true) override;
//		virtual VkPipelineDynamicStateCreateInfo prepare_dynamic_state() override;
//	};
//
//}} // namespace Albedo::Runtime