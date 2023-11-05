#include "skybox.h"
#include <fstream>
#include <Albedo.Core.Log>

namespace Albedo{
namespace APP
{
	static const char* vert_shader_path = "asset\\shader\\cubemap.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\skybox.frag.spv";
	
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

	SkyboxPipeline::
	SkyboxPipeline():
		GRI::GraphicsPipeline(GRI::GraphicsPipeline::ShaderModule
			{
				.descriptor_set_layouts =
				{
					*GRI::GetGlobalDescriptorSetLayout(GRI::MakeID({VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER})),
				},
				.vertex_shader   = GRI::Shader::Create(ShaderType_Vertex,	 ReadShader(vert_shader_path)),
				.fragment_shader = GRI::Shader::Create(ShaderType_Fragment,  ReadShader(frag_shader_path)),
			})
	{

	}

	void
	SkyboxPipeline::
	Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);
		/*vkCmdBindDescriptorSets(commandbuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			m_pipeline_layout, 0, 1, , 0);*/
		vkCmdDraw(*commandbuffer, GRI::Cubemap::VertexCount, 1, 0, 0);
	}

	void
	SkyboxPipeline::
	End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
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

}} // namespace Albedo::APP