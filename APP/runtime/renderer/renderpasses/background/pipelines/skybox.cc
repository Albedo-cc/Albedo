#include "skybox.h"
#include <fstream>
#include <Albedo.Core.Log>
#include <Albedo.Core.File>
#include <Albedo.Graphics.RHI>
#include <Albedo.Editor>

namespace Albedo{
namespace APP
{
	// ! NOT SKYBOX NOW ! (ONLY BACKGROUND)
	static const char* vert_shader_path = "asset\\shader\\postprocessing.vert.spv";
	static const char* frag_shader_path = "asset\\shader\\skybox.frag.spv";

	static Vector4D s_bgColor{ 0.0, 0.0, 0.0, 1.0 };
	static std::vector<VkPushConstantRange> push_constants
	{
		VkPushConstantRange
		{
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset		= 0,
			.size		= sizeof(s_bgColor),
		},
	};

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
			},push_constants)
	{
		Editor::RegisterUIEvent(new UIEvent
			{
				"Pipeline::Skybox",[]()->void
				{
					ImGui::Begin("Background");
					{
						ImGui::InputFloat4("Color", s_bgColor.data(), "%.1f");
					}
					ImGui::End();
				}
			});
	}

	void
	SkyboxPipeline::
	Begin(std::shared_ptr<CommandBuffer> commandbuffer)
	{
		Pipeline::Begin(commandbuffer);
		/*vkCmdBindDescriptorSets(commandbuffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			m_pipeline_layout, 0, 1, , 0);*/
		vkCmdPushConstants(
			*commandbuffer,
			m_pipeline_layout,
			VK_SHADER_STAGE_FRAGMENT_BIT,
			0, sizeof(s_bgColor), s_bgColor.data());
		vkCmdDraw(*commandbuffer, 3/*Cubemap::VertexCount*/, 1, 0, 0);
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