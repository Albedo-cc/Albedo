#include "skybox.h"
#include <fstream>
#include <Albedo.Core.Log>
#include <Albedo.Core.File>
#include <Albedo.Graphics.RHI>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "asset\\shader\\cubemap.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\skybox.frag.spv";

	SkyboxPipeline::
	SkyboxPipeline():
		GraphicsPipeline(GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{
					*RHI::GetGlobalDescriptorSetLayout(RHI::MakeID({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER})),
				},
				.vertex_shader   = Shader::Create(ShaderType_Vertex,	 BinaryFile(vert_shader_path)),
				.fragment_shader = Shader::Create(ShaderType_Fragment,  BinaryFile(frag_shader_path)),
			})
	{

	}

	void
	SkyboxPipeline::
	Begin(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);
		/*vkCmdBindDescriptorSets(commandbuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			m_pipeline_layout, 0, 1, , 0);*/
		//vkCmdDraw(*commandbuffer, Cubemap::VertexCount, 1, 0, 0);
	}

	void
	SkyboxPipeline::
	End(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		Pipeline::End(commandbuffer);
	}

	const VkPipelineVertexInputStateCreateInfo&
	SkyboxPipeline::
	vertex_input_state()
	{
		static VkVertexInputBindingDescription vertexInputBindingDescription
		{
			.binding = 0,
			.stride  = 3 * sizeof(float),
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

}} // namespace Albedo::APP