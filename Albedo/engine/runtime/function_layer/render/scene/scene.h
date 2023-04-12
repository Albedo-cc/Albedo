#pragma once

#include <AlbedoRHI.hpp>

#include <runtime/asset_layer/asset_layer_types.h>
#include "../paint_box/palette.h"

namespace Albedo {
namespace Runtime
{

	class Scene
	{
		friend class Easel;
		friend class Canvas;
	public:
		void Load(Easel& easel, std::shared_ptr<Model> model);

	public:
		Scene() = delete;
		Scene(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		std::shared_ptr<RHI::VMA::Buffer> m_vertices;
		std::shared_ptr<RHI::VMA::Buffer> m_indices;
		//std::vector<std::shared_ptr<RHI::Sampler>> samplers;		[ Future: now all of images share one default sampler ]
		std::vector<std::shared_ptr<RHI::VMA::Image>>m_images;		// (Objects)
		std::vector<Model::Texture> m_textures;
		std::vector<Model::Material> m_materials;
	};

}} // namespace Albedo::Runtime