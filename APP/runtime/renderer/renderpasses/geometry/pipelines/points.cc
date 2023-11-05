#include "points.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.File>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "asset\\shader\\opaque.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\opaque.frag.spv";

	PointsPipeline::
	PointsPipeline():
		GRI::GraphicsPipeline(GRI::GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{*GRI::GetGlobalDescriptorSetLayout("GlobalUBO_Camera")}, // Set=0
				.vertex_shader = 
				GRI::Shader::Create(ShaderType_Vertex,	 BinaryFile(vert_shader_path)),
				.fragment_shader = 
				GRI::Shader::Create(ShaderType_Fragment, BinaryFile(frag_shader_path)),
			})
	{

	}

	void
	PointsPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);
		vkCmdDraw(*commandbuffer, 6, 1, 0, 0);
	}

	void
	PointsPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		Pipeline::End(commandbuffer);
	}

	const VkPipelineVertexInputStateCreateInfo&
	PointsPipeline::
	vertex_input_state()
	{
		static VkVertexInputBindingDescription vertexInputBindingDescription
		{
			.binding = 0,
			.stride  = sizeof(Model::Vertex),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		static VkPipelineVertexInputStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 0,
			.pVertexBindingDescriptions   = nullptr,
			.vertexAttributeDescriptionCount = 0,
			.pVertexAttributeDescriptions = nullptr,
		};
		return state;
	}

	const VkPipelineInputAssemblyStateCreateInfo&
	PointsPipeline::
	input_assembly_state()
	{
		static VkPipelineInputAssemblyStateCreateInfo state
		{
			.sType		= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
			.topology	= VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
			.primitiveRestartEnable = VK_FALSE,
		};
		return state;
	}

	const VkPipelineDepthStencilStateCreateInfo&
	PointsPipeline::
	depth_stencil_state()
	{
		static VkPipelineDepthStencilStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
			.depthTestEnable		= VK_TRUE,
			.depthWriteEnable		= VK_TRUE,
			.depthCompareOp			= VK_COMPARE_OP_LESS, // Keep fragments, which has lower depth
			.depthBoundsTestEnable	= VK_FALSE,  // Only keep fragments that fall within the specified depth range.
			.stencilTestEnable		= VK_FALSE,
			.front = {},
			.back  = {},
			.minDepthBounds = 0.0,
			.maxDepthBounds = 1.0
		};
		return state;
	}

}} // namespace Albedo::APP