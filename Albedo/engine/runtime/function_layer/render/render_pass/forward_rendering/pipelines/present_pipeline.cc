#include "present_pipeline.h"
#include <runtime/function_layer/render/render_system_context.h>
#include <runtime/function_layer/render/model/model.h>

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{
	void PresentPipeline::Draw(std::shared_ptr<RHI::CommandBuffer> command_buffer)
	{
		assert(command_buffer->IsRecording() && "You cannot Draw() before beginning the command buffer!");

		// Prepare Data
		vkCmdSetViewport(*command_buffer, 0, m_viewports.size(), m_viewports.data());

		// Bind
		vkCmdBindPipeline(*command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
		auto& descriptorSets = m_descriptor_pool->GetAllDescriptorSets();
		vkCmdBindDescriptorSets(*command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout, 
														0, descriptorSets.size(), descriptorSets.data(),
														0, nullptr);
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

	void PresentPipeline::prepare_descriptor_sets()
	{
		m_descriptor_set_layouts.resize(MAX_DESCRIPTOR_SET_LAYOUT_COUNT);
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// 1. Create Descriptor Pool 
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		VkDescriptorPoolSize uniform_buffer_descriptor_set_size
		{
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = RenderSystemContext::MAX_FRAME_IN_FLIGHT
		};
		m_descriptor_pool = m_context->CreateDescriptorPool({ uniform_buffer_descriptor_set_size },
			RenderSystemContext::MAX_FRAME_IN_FLIGHT);
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		
		
		
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// 2. Descriptor Set Bindings
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// Binding: UBO
		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings(MAX_DESCRIPTOR_SET_COUNT);
		auto& ubo = descriptor_set_layout_bindings[descriptor_set_uniform_buffer];
		ubo.binding = descriptor_set_uniform_buffer;
		ubo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		ubo.descriptorCount = 1;
		ubo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		ubo.pImmutableSamplers = nullptr;
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// 3. Descriptor Set Layouts
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// Layout: UBO
		std::vector<VkDescriptorSetLayoutBinding> descriptor_set_layout_bindings_uniform_buffer
																					  { descriptor_set_layout_bindings[descriptor_set_uniform_buffer] };
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info_uniform_buffer
		{
			.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
			.bindingCount = static_cast<uint32_t>(descriptor_set_layout_bindings_uniform_buffer.size()),
			.pBindings = descriptor_set_layout_bindings_uniform_buffer.data()
		};
		if (vkCreateDescriptorSetLayout(m_context->m_device,
			&descriptor_set_layout_create_info_uniform_buffer,
			m_context->m_memory_allocation_callback,
			&m_descriptor_set_layouts[descriptor_set_layout_uniform_buffer]) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the Vulkan Descriptor Set Layout!");
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//



		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		// 4. Allocate and Write Descriptor Sets
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
		m_descriptor_pool->AllocateDescriptorSets(m_descriptor_set_layouts);
		// Uniform Buffers
		m_descriptor_pool->WriteBufferSet(descriptor_set_uniform_buffer, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			1, descriptor_set_uniform_buffer, current_frame_state.m_uniform_buffer, 0);
		// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------//
	}

	VkPipelineLayoutCreateInfo PresentPipeline::
		prepare_pipeline_layout_state()
	{
		return VkPipelineLayoutCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = static_cast<uint32_t>(m_descriptor_set_layouts.size()),
			.pSetLayouts = m_descriptor_set_layouts.data(),
			.pushConstantRangeCount = 0,
			.pPushConstantRanges = nullptr
		};
	}

	VkPipelineVertexInputStateCreateInfo	 PresentPipeline::
		prepare_vertex_input_state()
	{
		auto& bind_description = ModelVertex::GetBindingDescription(0);
		auto& attribute_description = ModelVertex::GetAttributeDescription(0);
		return VkPipelineVertexInputStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions = &bind_description,
			.vertexAttributeDescriptionCount = static_cast<uint32_t>(attribute_description.size()),
			.pVertexAttributeDescriptions = attribute_description.data()
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
		default_viewport.maxDepth = 1.0f;	// If you aren��t doing anything special, then you should stick to the standard values of 0.0f and 1.0f.

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
			.minSampleShading = 1.0f,
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