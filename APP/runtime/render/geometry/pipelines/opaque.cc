#include "opaque.h"
#include <fstream>
#include <Albedo.Core.Log>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "C:\\Frozen Zone\\MyGitHub\\Albedo\\APP\\asset\\shaders\\opaque.vert.spv";
	static const char* frag_shader_path = "C:\\Frozen Zone\\MyGitHub\\Albedo\\APP\\asset\\shaders\\opaque.frag.spv";
	
	static std::vector<char> ReadShader(const char* path)
	{
		std::vector<char> buffer;
		// Read File
		std::ifstream file(path, std::ios::ate | std::ios::binary);
		if (!file.is_open()) Log::Fatal("Failed to open the shader file {}!", path);

		size_t file_size = static_cast<size_t>(file.tellg());
		buffer.resize(file_size); 

		file.seekg(0);
		file.read(buffer.data(), file_size);

		file.close();

		return buffer;
	}

	OpaquePipeline::
	OpaquePipeline():
		GRI::GraphicsPipeline(GRI::GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{
					*GRI::GetGlobalDescriptorSetLayout("NULL") // Set=0
				},
				.vertex_shader   = GRI::Shader::Create(ShaderType_Vertex,	 ReadShader(vert_shader_path)),
				.fragment_shader = GRI::Shader::Create(ShaderType_Fragment,  ReadShader(frag_shader_path)),
			})
	{

	}

	void
	OpaquePipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot Begin() before beginning the command buffer!");
		vkCmdBindPipeline(*commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_handle);
		vkCmdDraw(*commandbuffer, 6, 1, 0, 0);
	}

	void
	OpaquePipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		assert(commandbuffer->IsRecording() && "You cannot End() before beginning the command buffer!");
	}

	const VkPipelineVertexInputStateCreateInfo&
	OpaquePipeline::
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
	OpaquePipeline::
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

	/*VkPipelineDynamicStateCreateInfo
	OpaquePipeline::
	dynamic_state()
	{
		static VkDynamicState dynamicStates[]
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_LINE_WIDTH,
		};
		return VkPipelineDynamicStateCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
			.dynamicStateCount = 2,
			.pDynamicStates = dynamicStates,
		};
	}*/

}} // namespace Albedo::APP