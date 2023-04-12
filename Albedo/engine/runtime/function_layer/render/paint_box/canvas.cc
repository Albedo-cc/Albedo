#include "canvas.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	void Canvas::BeginPainting(std::shared_ptr<RHI::RenderPass> renderPass)
	{
		command_buffer->Begin();
		renderPass->Begin(command_buffer);
		//return command_buffer;
	}

	void	Canvas::Paint(RHI::GraphicsPipeline* brush, Scene& scene)
	{
		brush->Bind(command_buffer);

		VkBuffer VBO = *scene.m_vertices;
		VkBuffer IBO = *scene.m_indices;
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(*command_buffer, 0, 1, &VBO, offsets);
		vkCmdBindIndexBuffer(*command_buffer, IBO, 0, VK_INDEX_TYPE_UINT32);

		// Bind Descriptor Sets
		std::vector<VkDescriptorSet> boundSets
		{
			*palette.SET0_ubo,
			*palette.SET1_texture
		};

		vkCmdBindDescriptorSets(*command_buffer,
			VK_PIPELINE_BIND_POINT_GRAPHICS,
			brush->GetPipelineLayout(),
			0, boundSets.size(), boundSets.data(),
			0, nullptr);

		// Paint Scene

	}

	void Canvas::EndPainting(std::shared_ptr<RHI::RenderPass> renderPass)
	{
		renderPass->End(command_buffer);
		command_buffer->End();
		command_buffer->Submit(false, *syncmeta.fence_in_flight,
			{ *syncmeta.semaphore_image_available },
			{ *syncmeta.semaphore_render_finished },
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	}

	void Canvas::paint_model_node(RHI::GraphicsPipeline* brush, std::shared_ptr<Model::Node> model_node)
	{

	}

}} // namespace Albedo::Runtime