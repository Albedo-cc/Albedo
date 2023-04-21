#include "canvas.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	void	Canvas::Paint(std::shared_ptr<RHI::CommandBuffer> commandBuffer, RHI::GraphicsPipeline* brush, std::shared_ptr<Scene> scene)
	{
		assert(commandBuffer->IsRecording() && "You must Begin() command Buffer before Paint()!");

		brush->Bind(commandBuffer);

		VkBuffer VBO = *scene->vertices;
		VkBuffer IBO = *scene->indices;
		VkDeviceSize offsets[1] = { 0 };
		vkCmdBindVertexBuffers(*commandBuffer, 0, 1, &VBO, offsets);
		vkCmdBindIndexBuffer(*commandBuffer, IBO, 0, VK_INDEX_TYPE_UINT32);

		// Paint Scene
		for (auto& node : scene->nodes) paint_model_node(brush, *scene, node);
	}

	void Canvas::paint_model_node(RHI::GraphicsPipeline* brush, Scene& scene, std::shared_ptr<Model::Node> model_node)
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
			vkCmdPushConstants(*cmd_buffer_front, 
				brush->GetPipelineLayout(),
				VK_SHADER_STAGE_VERTEX_BIT, 
				0, sizeof(glm::mat4), &nodeMatrix);

			for (auto& primitive : model_node->mesh.primitives) 
			{
				if (primitive.index_count > 0) 
				{
					// Bind Descriptor Sets
					Palette::BindDescriptorSetMaterial(
						cmd_buffer_front, 
						brush, 
						scene.m_descriptor_set_materials[primitive.material_index]);

					vkCmdDrawIndexed(*cmd_buffer_front, primitive.index_count, 1, primitive.first_index, 0, 0);
				}
			}
		}
		for (auto& child : model_node->children) paint_model_node(brush, scene, child);
	}

}} // namespace Albedo::Runtime