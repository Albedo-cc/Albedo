#include "present_pipeline.h"

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{
	void PresentPipeline::Draw(RHI::CommandPool::CommandBuffer& command_buffer)
	{
		assert(command_buffer.IsRecording() && "You cannot Draw() before beginning the command buffer!");

		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		vkCmdSetViewport(command_buffer, 0, m_viewports.size(), m_viewports.data());
		vkCmdDraw(command_buffer, 3, 1, 0, 0);
	}

	PresentPipeline::PresentPipeline(
		std::shared_ptr<RHI::VulkanContext> vulkan_context,
		VkRenderPass owner,
		uint32_t subpass_bind_point,
		VkPipeline base_pipeline/* = VK_NULL_HANDLE*/, 
		int32_t base_pipeline_index/* = -1*/):
		RHI::GraphicsPipeline{ vulkan_context , owner, subpass_bind_point}
	{
		Initialize();
	}

	std::vector<VkPipelineShaderStageCreateInfo>	 PresentPipeline::
		prepare_shader_stage_state()
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderInfos(2);

		// Vertex Shader
		auto vertex_shader = create_shader_module("resource/shader/default.vert.spv");
		auto& vertex_shader_info = shaderInfos[0];
		vertex_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertex_shader_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertex_shader_info.module = vertex_shader;
		vertex_shader_info.pName = "main";

		// Fragment Shader
		auto fragment_shader = create_shader_module("resource/shader/default.frag.spv");
		auto& fragment_shader_info = shaderInfos[1];
		fragment_shader_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragment_shader_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragment_shader_info.module = fragment_shader;
		fragment_shader_info.pName = "main";

		return shaderInfos;
	}

	VkPipelineLayoutCreateInfo PresentPipeline::
		prepare_pipeline_layout_state()
	{
		return VkPipelineLayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
	}

	VkPipelineVertexInputStateCreateInfo	 PresentPipeline::
		prepare_vertex_inpute_state()
	{
		return VkPipelineVertexInputStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr
		};
	}

	VkPipelineInputAssemblyStateCreateInfo PresentPipeline::
		prepare_input_assembly_state()
	{
		return VkPipelineInputAssemblyStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};
	}

	VkPipelineViewportStateCreateInfo	PresentPipeline::
		prepare_viewport_state()
	{
		auto& default_viewport = m_viewports.emplace_back();
		default_viewport.x = 0.0f;
		default_viewport.y = 0.0f;
		default_viewport.width = static_cast<float>(m_context->m_swapchain_current_extent.width);
		default_viewport.height = static_cast<float>(m_context->m_swapchain_current_extent.height);
		default_viewport.minDepth = 0.0f;	// minDepth may be higher than maxDepth
		default_viewport.maxDepth = 1.0f;	// If you aren¡¯t doing anything special, then you should stick to the standard values of 0.0f and 1.0f.

		auto& default_scissor = m_scissors.emplace_back();
		default_scissor.offset = { 0,0 };
		default_scissor.extent = m_context->m_swapchain_current_extent;

		return VkPipelineViewportStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = static_cast<uint32_t>(m_viewports.size()),
			.pViewports = m_viewports.data(),
			.scissorCount = static_cast<uint32_t>(m_scissors.size()),
			.pScissors = m_scissors.data()
		};
	}

	VkPipelineRasterizationStateCreateInfo PresentPipeline::
		prepare_rasterization_state()
	{
		return VkPipelineRasterizationStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable = VK_FALSE, // Fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
			.rasterizerDiscardEnable = VK_FALSE, // if VK_TRUE, then geometry never passes through the rasterizer stage
			.polygonMode = VK_POLYGON_MODE_FILL,
			.cullMode = VK_CULL_MODE_BACK_BIT,
			.frontFace = VK_FRONT_FACE_CLOCKWISE,
			.depthBiasEnable = VK_FALSE,
			.depthBiasConstantFactor = 0.0f,
			.depthBiasClamp = 0.0f,
			.depthBiasSlopeFactor = 0.0f, // This is sometimes used for shadow mapping
			.lineWidth = 1.0f // Any line thicker than 1.0f requires you to enable the wideLines GPU feature.			
		};
	}

	VkPipelineMultisampleStateCreateInfo PresentPipeline::
		prepare_multisampling_state()
	{
		return VkPipelineMultisampleStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
			.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
			.sampleShadingEnable = VK_FALSE,
			. minSampleShading = 1.0f,
			.pSampleMask = nullptr,
			.alphaToCoverageEnable = VK_FALSE,
			.alphaToOneEnable = VK_FALSE
		};
	}

	VkPipelineDepthStencilStateCreateInfo PresentPipeline::
		prepare_depth_stencil_state()
	{
		return VkPipelineDepthStencilStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable = VK_FALSE,
			.depthWriteEnable = VK_FALSE,
			.depthBoundsTestEnable = VK_FALSE,
			.stencilTestEnable = VK_FALSE
		};
	}

	VkPipelineColorBlendStateCreateInfo PresentPipeline::
		prepare_color_blend_state()
	{
		static VkPipelineColorBlendAttachmentState colorBlendAttachment
		{
				.blendEnable = VK_TRUE,
				.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
				.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
				.colorBlendOp = VK_BLEND_OP_ADD,
				.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
				.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
				.alphaBlendOp = VK_BLEND_OP_ADD,
				.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
		};
		return VkPipelineColorBlendStateCreateInfo
		{
				.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
				.logicOpEnable = VK_FALSE, // VK_FALSE: Mix Mode | VK_TRUE: Combine Mode
				/* P115
				The bitwise operation can then be specified in the logicOp field.
				Note that this will automatically disable the first method*/
				.logicOp = VK_LOGIC_OP_COPY,
				.attachmentCount = 1,
				.pAttachments = &colorBlendAttachment,
				.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
		};
	}

	VkPipelineDynamicStateCreateInfo	PresentPipeline::
		prepare_dynamic_state()
	{
		static std::vector<VkDynamicState> dynamicStates
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH
		};

		return VkPipelineDynamicStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()),
			.pDynamicStates = dynamicStates.data()
		};
	}

}} // namespace Albedo::Runtime