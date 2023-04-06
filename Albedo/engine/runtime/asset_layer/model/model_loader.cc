#include <AlbedoLog.hpp>
#include <AlbedoRHI.hpp>

#include "model_loader.h"

//----------------------------------------------------------------------------------------------------------------//
// Model Format: glTF
//----------------------------------------------------------------------------------------------------------------//
#define TINYGLTF_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION (Defined in image_loader.cc)
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_USE_CPP14
#include <tiny_gltf.h>
//----------------------------------------------------------------------------------------------------------------//

#include <cstring>

namespace Albedo {
namespace Runtime
{
	std::shared_ptr<SModel> ModelLoader::
		LoadModel(std::string_view model_path, AssetUsageFlags usage)
	{
		auto suffix = model_path.substr(model_path.rfind('.'));
		if (".gltf" == suffix)
		{
			return load_model_glTF(model_path, usage);
		}
		else throw std::runtime_error("Failed to load the model - Unsupported Model Format!");

		return nullptr;
	}

	std::shared_ptr<SModel> ModelLoader::
		load_model_glTF(std::string_view model_path, AssetUsageFlags usage)
	{
		auto model = std::make_shared<SModel>();
		tinygltf::TinyGLTF glTFContext;
		tinygltf::Model glTFModel;
		std::string error, warning;

		if (glTFContext.LoadASCIIFromFile(&glTFModel, &error, &warning, model_path.data()))
		{
			if (glTFModel.scenes.size() > 1)
				log::warn("Model {} has {} scenes, however, only the 1st scene was loaded by default!", model_path, glTFModel.scenes.size());

			// 0. Load Nodes
			static auto func_load_node =
				[&](const tinygltf::Node& src_node, std::shared_ptr<SModel::Node> parent, auto&& _func_load_node)->void
			{
				auto node = std::make_shared<SModel::Node>();
				node->parent = parent;
				node->matrix.setIdentity();
				auto node_matrix_linear_block = node->matrix.block(0, 0, 3, 3);
				// Get the local node matrix, it's either made up from translation, rotation, scale or a 4x4 matrix
				if (src_node.matrix.size() == 16)
				{
					std::vector<float> vec(src_node.matrix.begin(), src_node.matrix.end());
					node->matrix = Matrix4f(vec.data()); // Cannot convert double vector to Matrix4f directly.
				}
				else
				{
					if (src_node.translation.size() == 3)
					{
						node->matrix(0, 3) = src_node.translation[0];
						node->matrix(1, 3) = src_node.translation[1];
						node->matrix(2, 3) = src_node.translation[2];
					}
					if (src_node.rotation.size() == 4)
					{
						Quaternionf quaternion(src_node.rotation[0], src_node.rotation[1], src_node.rotation[2], src_node.rotation[3]);
						node_matrix_linear_block = quaternion.normalized().toRotationMatrix() * node_matrix_linear_block;
					}
					if (src_node.scale.size() == 3)
					{
						node_matrix_linear_block(0, 0) *= src_node.scale[0];
						node_matrix_linear_block(1, 1) *= src_node.scale[1];
						node_matrix_linear_block(2, 2) *= src_node.scale[2];
					}
				} // End load matrix

				// Load children recursively
				if (!src_node.children.empty())
				{
					for (auto& child : src_node.children) 
						_func_load_node(glTFModel.nodes[child], node->shared_from_this(), _func_load_node);
				}

				if (src_node.mesh >= 0)
				{
					auto& mesh = glTFModel.meshes[src_node.mesh];
					
					for (size_t i = 0; i < mesh.primitives.size(); ++i)
					{
						auto& primitive = mesh.primitives[i];
						// Load all of Vertices and Indices in 2 separate buffers.
						SModel::VertexIndex first_index = static_cast<SModel::VertexIndex>(model->indices.size());
						uint32_t vertex_start = static_cast<uint32_t>(model->vertices.size());
						size_t index_count = 0;

						// Load Vertices
						{
							size_t vertex_count = 0;
							const float* buffer_position = nullptr;
							const float* buffer_normals = nullptr;
							const float* buffer_texCoords= nullptr;
							// [Attribute] Position
							auto attribute_position = primitive.attributes.find("POSITION");
							if (attribute_position != primitive.attributes.end())
							{
								auto& accessor = glTFModel.accessors[attribute_position->second];
								auto& buffer_view = glTFModel.bufferViews[accessor.bufferView];
								buffer_position = reinterpret_cast<const float*>(&(glTFModel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]));
								vertex_count = accessor.count;
							}
							// [Attribute] Normal
							auto attribute_normal = primitive.attributes.find("NORMAL");
							if (attribute_normal != primitive.attributes.end())
							{
								auto& accessor = glTFModel.accessors[attribute_normal->second];
								auto& buffer_view = glTFModel.bufferViews[accessor.bufferView];
								buffer_normals = reinterpret_cast<const float*>(&(glTFModel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]));
							}
							// [Attribute] Texture Coordinate
							auto attribute_textCoord = primitive.attributes.find("TEXCOORD_0");
							if (attribute_textCoord != primitive.attributes.end())
							{
								auto& accessor = glTFModel.accessors[attribute_textCoord->second];
								auto& buffer_view = glTFModel.bufferViews[accessor.bufferView];
								buffer_texCoords = reinterpret_cast<const float*>(&(glTFModel.buffers[buffer_view.buffer].data[accessor.byteOffset + buffer_view.byteOffset]));
							}

							model->vertices.resize(vertex_count);
							for (size_t i =0; i< vertex_count; ++i)
							{
								auto& vertex = model->vertices[i];
								vertex.position = Vector3f(&buffer_position[i * 3]);
								vertex.normal = buffer_normals ? Vector3f(&buffer_normals[i * 3]) : Vector3f(0, 0, 0);
								vertex.uv = buffer_texCoords ? Vector2f(&buffer_texCoords[i * 2]) : Vector2f(0, 0);
								vertex.color = Vector4f(1.0, 1.0, 1.0, 1.0);
							}
						}
						// Load Indices
						{
							auto& accessor = glTFModel.accessors[primitive.indices];
							auto& buffer_view = glTFModel.bufferViews[accessor.bufferView];
							auto& buffer = glTFModel.buffers[buffer_view.buffer];
							index_count += accessor.count;

							// glTF supports different component types of indices
							switch (accessor.componentType)
							{
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT:
								{
									const uint32_t* index_buffer = reinterpret_cast<const uint32_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++)
										model->indices.push_back(vertex_start + index_buffer[index]);
									break;
								}
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT:
								{
									const uint16_t* index_buffer = reinterpret_cast<const uint16_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++)
										model->indices.push_back(vertex_start + index_buffer[index]);
									break;
								}
								case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE:
								{
									const uint8_t* index_buffer = reinterpret_cast<const uint8_t*>(&buffer.data[accessor.byteOffset + buffer_view.byteOffset]);
									for (size_t index = 0; index < accessor.count; index++)
										model->indices.push_back(vertex_start + index_buffer[index]);
									break;
								}
								default: throw std::runtime_error("Failed to load model - Unkonw Component Type!");
							}

							auto& mesh_primitive = node->mesh.primitives.emplace_back();
							mesh_primitive.first_index = first_index;
							mesh_primitive.index_count = index_count;
							mesh_primitive.material_index = primitive.material;
						}
					} // End interate primitives
				} // End interate meshes
				if (parent != nullptr) parent->children.emplace_back(node);
				else model->nodes.emplace_back(node);
			}; // End (func_load_node)

			auto& scene = glTFModel.scenes.front();
			for (size_t i = 0; i < scene.nodes.size(); ++i)
			{
				auto& node = glTFModel.nodes[scene.nodes[i]];
				func_load_node(node, nullptr, func_load_node);
			}

			if (ASSET_USAGE_RENDER_BIT & usage)
			{
				// 1. Load Images
				model->images.resize(glTFModel.images.size());
				for (size_t i = 0; i < glTFModel.images.size(); ++i)
				{
					auto& src_image = glTFModel.images[i];
					auto& dst_image = model->images[i];
					dst_image.width		= src_image.width;
					dst_image.height		= src_image.height;
					dst_image.channel	= src_image.component;
					size_t image_area = static_cast<VkDeviceSize>(dst_image.width) * dst_image.height;
					VkDeviceSize buffer_size = image_area * 4;
					
					dst_image.data = new uint8_t[buffer_size];
					unsigned char* dst_data = dst_image.data;
					unsigned char* src_data = src_image.image.data();
					if (src_image.component != 4)
					{
						for (size_t i = 0; i < image_area; ++i)
						{
							memcpy(dst_data, src_data, sizeof(uint8_t) * src_image.component);
							dst_data += 4; // RGBA
							src_data += src_image.component;
						}
					}
					else memcpy(dst_data, src_data, buffer_size);
				}

				// 2. Load Materials
				model->materials.resize(glTFModel.materials.size());
				for (size_t i = 0; i < glTFModel.materials.size(); ++i)
				{
					auto& material = glTFModel.materials[i];
					// Get base color factor
					if (material.values.find("baseColorFactor") != material.values.end())
					{
						auto base_color_factor = material.values["baseColorFactor"].ColorFactor();
						std::vector<float> baseColor(base_color_factor.begin(), base_color_factor.end());
						model->materials[i].base_color_factor = Vector4f(baseColor.data());
					}
					// Get base color texture index
					if (material.values.find("baseColorTexture") != material.values.end())
					{
						model->materials[i].base_color_texture_index = material.values["baseColorTexture"].TextureIndex();
					}
				}

				// 3. Load Textures
				model->textures.resize(glTFModel.textures.size());
				for (size_t i = 0; i < glTFModel.textures.size(); ++i)
				{
					auto& texture = glTFModel.textures[i];
					model->textures[i].imageIndex = texture.source;
				}
			}
		}
		else throw std::runtime_error("Failed to load model!\nError: " + error + "\nWarn: " + warning);

		return model;
	}

}} // namespace Albedo::Runtime