#include "front_render_pipeline.h"

#include "../../RHI/Vulkan/vulkan_manager.h"

namespace Albedo {
namespace Runtime
{

	FrontRenderPipeline::FrontRenderPipeline(std::shared_ptr<RHI::VulkanManager> vulkan_manager)
	{
		m_vulkan_manager = std::move(vulkan_manager);

		// Shader Stages
		std::array<VkPipelineShaderStageCreateInfo, MAX_SHADER_STAGE> shaderStages{};
		shaderStages[VERTEX_STAGE]			= prepare_vertex_shader_stage("resource/shader/default.vert.spv");
		shaderStages[FRAGMENT_STAGE]	= prepare_fragment_shader_stage("resource/shader/default.frag.spv");

		// Pipeline Layout
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount  = 0,
			.pSetLayouts = nullptr,
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};

		// Fixed State
		// 1. Vertex Input	
		auto vertexInputState = prepare_vertex_inpute_state();
		// 2. Input Assembly 
		auto inputAssemblyState = prepare_input_assembly_state();
		// 3. Viewports and Scissors - A viewport basically describes the region of the framebuffer that the output will be rendered to.
		auto viewportState = prepare_viewport_state();
		// 4. Rasterizer
		auto rasterizationState = prepare_rasterization_state();
		// 5. Multisampling (Enabling it requires enabling a GPU feature)
		auto multisamplingState = prepare_multisampling_state();
		// 6. Depth and Stencil Testing
		auto depthStencilState = nullptr;
		// 7. Color Blending
		auto colorBlendState = prepare_color_blend_state();

		// Dynamic State
		auto dynamicState = prepare_dynamic_state();

		// Pipeline Layout
	}

	VkPipelineShaderStageCreateInfo FrontRenderPipeline::
		prepare_vertex_shader_stage(std::string_view vertex_shader)
	{
		m_shader_stages[VERTEX_STAGE] = m_vulkan_manager->CreateShaderModule(vertex_shader);
		return VkPipelineShaderStageCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_VERTEX_BIT,
			.module = m_shader_stages[VERTEX_STAGE],
			.pName = "main" // [Entrypoint] it¡¯s possible to combine multiple fragment shaders into a single shader module
		};
	}

	VkPipelineShaderStageCreateInfo FrontRenderPipeline::
		prepare_fragment_shader_stage(std::string_view fragment_shader)
	{
		m_shader_stages[FRAGMENT_STAGE] = m_vulkan_manager->CreateShaderModule(fragment_shader);
		return VkPipelineShaderStageCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
			.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
			.module = m_shader_stages[FRAGMENT_STAGE],
			.pName = "main" // [Entrypoint] it¡¯s possible to combine multiple fragment shaders into a single shader module
		};
	}

	VkPipelineVertexInputStateCreateInfo FrontRenderPipeline::
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

	VkPipelineInputAssemblyStateCreateInfo FrontRenderPipeline::
		prepare_input_assembly_state()
	{
		return VkPipelineInputAssemblyStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
			.primitiveRestartEnable = VK_FALSE
		};
	}

	VkPipelineViewportStateCreateInfo FrontRenderPipeline::
		prepare_viewport_state()
	{
		static VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_vulkan_manager->GetVulkanContext().m_swapchain_current_extent.width);
		viewport.height = static_cast<float>(m_vulkan_manager->GetVulkanContext().m_swapchain_current_extent.height);
		viewport.minDepth = 0.0f;	// minDepth may be higher than maxDepth
		viewport.maxDepth = 1.0f;	// If you aren¡¯t doing anything special, then you should stick to the standard values of 0.0f and 1.0f.

		static VkRect2D scissor{};
		scissor.offset = { 0,0 };
		scissor.extent = m_vulkan_manager->GetVulkanContext().m_swapchain_current_extent;

		return VkPipelineViewportStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
			.viewportCount = 1,
			.pViewports = &viewport,
			.scissorCount = 1,
			.pScissors = &scissor
		};
	}

	VkPipelineRasterizationStateCreateInfo FrontRenderPipeline::
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

	VkPipelineMultisampleStateCreateInfo FrontRenderPipeline::
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

	VkPipelineColorBlendStateCreateInfo FrontRenderPipeline::
		prepare_color_blend_state(bool is_mix_mode/* = true*/)
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
				.logicOpEnable = is_mix_mode ? VK_FALSE : VK_TRUE,
				/* P115
				The bitwise operation can then be specified in the logicOp field.
				Note that this will automatically disable the first method*/
				.logicOp = VK_LOGIC_OP_COPY,
				.attachmentCount = 1,
				.blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}
		};
	}

	VkPipelineDynamicStateCreateInfo FrontRenderPipeline::
		prepare_dynamic_state()
	{
		static std::array<VkDynamicState, 2> dynamicStates 
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