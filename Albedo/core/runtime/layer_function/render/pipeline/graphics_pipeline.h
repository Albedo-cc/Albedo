#pragma once

#include "../RHI/Vulkan/vulkan_manager.h"

#include <memory>
#include <string_view>
#include <array>

namespace Albedo {
namespace Runtime
{

	class GraphicsPipeline
	{
	public:
		virtual void render() = 0;

		virtual void init()
		{

		}

	protected:
		std::shared_ptr<RHI::VulkanManager> m_vulkan_manager;
		VkPipelineLayout m_pipeline_layout;

		enum ShaderStages { VERTEX_STAGE, FRAGMENT_STAGE, MAX_SHADER_STAGE };
		std::array<VkShaderModule, MAX_SHADER_STAGE> m_shader_stages;

	protected:
		virtual VkPipelineShaderStageCreateInfo prepare_vertex_shader_stage(std::string_view vertex_shader) = 0;
		virtual VkPipelineShaderStageCreateInfo prepare_fragment_shader_stage(std::string_view fragment_shader) = 0;

		virtual VkPipelineVertexInputStateCreateInfo prepare_vertex_inpute_state() = 0;
		virtual VkPipelineInputAssemblyStateCreateInfo prepare_input_assembly_state() = 0;
		virtual VkPipelineViewportStateCreateInfo prepare_viewport_state() = 0;
		virtual VkPipelineRasterizationStateCreateInfo prepare_rasterization_state() = 0;
		virtual VkPipelineMultisampleStateCreateInfo prepare_multisampling_state() = 0;
		// prepare_depth_and_stencil_state();
		virtual VkPipelineColorBlendStateCreateInfo prepare_color_blend_state(bool is_mix_mode = true) = 0;
		virtual VkPipelineDynamicStateCreateInfo prepare_dynamic_state() = 0;
	};

}} // namespace Albedo::Runtime