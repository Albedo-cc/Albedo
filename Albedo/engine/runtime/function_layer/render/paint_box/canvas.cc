#include "canvas.h"
#include "easel.h"

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

	void	Canvas::Paint(RHI::GraphicsPipeline* brush, Easel::Scene& scene)
	{
		brush->Bind(command_buffer);

		VkBuffer VBO = *scene.vertices;
		VkBuffer IBO = *scene.indices;
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
		if (model_node->mesh.primitives.size() > 0) {
			// Pass the node's matrix via push constants
			// Traverse the node hierarchy to the top-most parent to get the final matrix of the current node
			auto& nodeMatrix = model_node->matrix;
			auto currentParent = model_node->parent.lock();

			while (currentParent) 
			{
				nodeMatrix = currentParent->matrix * nodeMatrix;
				currentParent = currentParent->parent.lock();
			}

			// Pass the final matrix to the vertex shader using push constants
			/*vkCmdPushConstants(command_buffer, 
				brush->GetDescriptorSetLayout(), 
				VK_SHADER_STAGE_VERTEX_BIT, 
				0, sizeof(glm::mat4), &nodeMatrix);*/

			for (auto& primitive : model_node->mesh.primitives) 
			{
				if (primitive.index_count > 0) 
				{
					// Get the texture index for this primitive
					if (!textures.empty())
					{
						VulkanglTFModel::Texture texture = textures[materials[primitive.materialIndex].baseColorTextureIndex];
						// Bind the descriptor for the current primitive's texture
						vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &images[texture.imageIndex].descriptorSet, 0, nullptr);
					}
					vkCmdDrawIndexed(command_buffer, primitive.indexCount, 1, primitive.firstIndex, 0, 0);
				}
			}
		}
		for (auto& child : model_node->children) paint_model_node(brush, child);
	}

}} // namespace Albedo::Runtime