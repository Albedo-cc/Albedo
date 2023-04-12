#pragma once

#include "canvas.h"

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{
	class Model;

	class Easel
	{
	public:
		using CID = size_t; // Canvas ID
		static constexpr CID MAX_CANVAS_COUNT = 2; // MAX IN-FLIGHT FRAMES

		struct Scene
		{
			std::shared_ptr<RHI::VMA::Buffer> vertices;
			std::shared_ptr<RHI::VMA::Buffer> indices;
			//std::vector<std::shared_ptr<RHI::Sampler>> samplers;	[ Future: now all of images share one default sampler ]
			std::vector<std::shared_ptr<RHI::VMA::Image>>images;
			std::vector<Model::Texture> textures;
			std::vector<Model::Material> materials;
			std::vector<std::shared_ptr<Model::Node>> nodes;
		};

	public:
		// You must get or present canvas via an easel.
		void SetupTheScene(std::shared_ptr<Model> scene);
		Canvas& WaitCanvas() throw (RHI::VulkanContext::swapchain_error);
		void PresentCanvas(bool switch_canvas = true) throw (RHI::VulkanContext::swapchain_error);

	public:
		Easel() = delete;
		Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<RHI::CommandPool> m_command_pool; // Resetable
		std::shared_ptr<RHI::DescriptorPool> m_descriptor_pool;

		std::unique_ptr<Scene> m_scene;

		CID m_current_canvas = 0;
		std::vector<Canvas> m_canvases;
	};

}} // namespace Albedo::Runtime