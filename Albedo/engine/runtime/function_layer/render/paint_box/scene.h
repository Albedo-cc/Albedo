#pragma once

#include <AlbedoRHI.hpp>

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{

	class Scene
	{
		friend class Canvas; // Painted by Canvas
	public:
		void Sketch(std::shared_ptr<Model> model); // Load Model

	public:
		Scene() = delete;
		Scene(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
			m_vulkan_context{ std::move(vulkan_context) } {}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		std::shared_ptr<RHI::VMA::Buffer> vertices;
		std::shared_ptr<RHI::VMA::Buffer> indices;
		//std::vector<std::shared_ptr<RHI::Sampler>> samplers;	[ Future: now all of images share one default sampler ]
		std::vector<std::shared_ptr<RHI::VMA::Image>>images;
		std::vector<Model::Texture> textures;
		std::vector<std::shared_ptr<RHI::DescriptorSet>> m_descriptor_set_materials;
		std::vector<Model::Material> materials;
		std::vector<std::shared_ptr<Model::Node>> nodes;
	};

}} // namespace Albedo::Runtime