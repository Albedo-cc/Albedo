#include "triangles.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.File>

#include <runtime/renderer/renderer.h>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "asset\\shader\\opaque.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\opaque.frag.spv";

	TrianglesPipeline::
	TrianglesPipeline():
		GRI::GraphicsPipeline(GRI::GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{
					*GRI::GetGlobalDescriptorSetLayout("GlobalUBO_Camera") // Set=0
				},
				.vertex_shader = 
				GRI::Shader::Create(ShaderType_Vertex,	 BinaryFile(vert_shader_path)),
				.fragment_shader = 
				GRI::Shader::Create(ShaderType_Fragment, BinaryFile(frag_shader_path)),
			})
	{

	}

	void
	TrianglesPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);

		auto& ctx = Renderer::GetInstance().GetFrameContext();

		std::vector<VkDescriptorSet> descriptorSets
		{
			*ctx.ubo.lock(),
		};
		
		uint32_t uniform_offset = GRI::PadUniformBufferSize(sizeof(GlobalUBO))* ctx.frame_index;
		vkCmdBindDescriptorSets(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout,
			0, descriptorSets.size(), descriptorSets.data(), 1, &uniform_offset);


		vkCmdDraw(*commandbuffer, 6, 1, 0, 0);
	}

	void
	TrianglesPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		Pipeline::End(commandbuffer);
	}

	const VkPipelineVertexInputStateCreateInfo&
	TrianglesPipeline::
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

	const VkPipelineDepthStencilStateCreateInfo&
	TrianglesPipeline::
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