#include "triangles.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.File>
#include <Albedo.Graphics.RHI>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "asset\\shader\\opaque.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\opaque.frag.spv";

	TrianglesPipeline::
	TrianglesPipeline():
		GraphicsPipeline(GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{
					*RHI::GetGlobalDescriptorSetLayout("GlobalUBO_Camera") // Set=0
				},
				.vertex_shader = 
				Shader::Create(ShaderType_Vertex,	 BinaryFile(vert_shader_path)),
				.fragment_shader = 
				Shader::Create(ShaderType_Fragment, BinaryFile(frag_shader_path)),
			})
	{

	}

	void
	TrianglesPipeline::
	Begin(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);

		auto& ctx = Renderer::GetInstance().GetFrameContext();

		std::vector<VkDescriptorSet> descriptorSets
		{
			*ctx.ubo.lock(),
		};
		
		uint32_t uniform_offset = RHI::PadUniformBufferSize(sizeof(GlobalUBO))* ctx.frame_index;
		vkCmdBindDescriptorSets(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline_layout,
			0, descriptorSets.size(), descriptorSets.data(), 1, &uniform_offset);

		VkBuffer vbo = *ctx.model->vbo;
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(*commandbuffer, 0, 1, &vbo, offsets);
		// [TODO]: Model::Draw?
		ALBEDO_ASSERT(ctx.model->data.vertices.count % 3 == 0);
		vkCmdDraw(*commandbuffer, ctx.model->data.vertices.count / 3, 1, 0, 0);
	}

	void
	TrianglesPipeline::
	End(std::shared_ptr<CommandBuffer> commandbuffer)
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
			.stride  = 3 * sizeof(float),
			.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
		};

		static VkVertexInputAttributeDescription positionAttribute
		{
			.location = 0,
			.binding = 0,
			.format = VK_FORMAT_R32G32B32_SFLOAT,
			.offset = 0 //offsetof(Vertex, position);
		};

		static VkPipelineVertexInputStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
			.vertexBindingDescriptionCount = 1,
			.pVertexBindingDescriptions   = &vertexInputBindingDescription,
			.vertexAttributeDescriptionCount = 1,
			.pVertexAttributeDescriptions = &positionAttribute,
		};
		return state;
	}

	const VkPipelineRasterizationStateCreateInfo&
	TrianglesPipeline::
	rasterization_state()
	{
		static VkPipelineRasterizationStateCreateInfo state
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
			.depthClampEnable		= VK_FALSE, // Fragments that are beyond the near and far planes are clamped to them as opposed to discarding them
			.rasterizerDiscardEnable= VK_FALSE, // if VK_TRUE, then geometry never passes through the rasterizer stage
			.polygonMode			= VK_POLYGON_MODE_LINE,
			.cullMode				= VK_CULL_MODE_BACK_BIT,
			.frontFace				= VK_FRONT_FACE_CLOCKWISE, // Consistent with Unity Engine. (Note Y-Flip)
			.depthBiasEnable		= VK_TRUE,
			.depthBiasConstantFactor= 0.0f, 
			.depthBiasClamp			= 0.0f,
			.depthBiasSlopeFactor	= 0.0f, // This is sometimes used for shadow mapping
			.lineWidth				= 1.0f // Any line thicker than 1.0f requires you to enable the wideLines GPU feature.			
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
			.depthCompareOp			= VK_COMPARE_OP_GREATER, // Z-Reversed
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