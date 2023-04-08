#pragma once

#include "palette.h"

#include <runtime/function_layer/render/camera/camera.h>
#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	class Canvas // One Frame
	{
		using SID = size_t;
		class Scene
		{
		public:
			std::shared_ptr<RHI::VMA::Buffer> vertex_buffer;
			std::shared_ptr<RHI::VMA::Buffer> index_buffer;
			std::vector<std::shared_ptr<RHI::Sampler>> samplers; // Future
			std::vector<std::shared_ptr<RHI::VMA::Image>> textures;
		private:
			std::unique_ptr<Palette> m_palette;
		};

	public:
		void Render(SID scene_id, std::shared_ptr<RHI::CommandBuffer> commandBuffer, RHI::GraphicsPipeline* pipeline); // Render all Scenes and their nodes (Future: Trim)

		SID LoadScene(std::shared_ptr<SModel> scene, std::shared_ptr<RHI::DescriptorPool> descriptorPool);

	public:
		Canvas() = delete;
		Canvas(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::vector<Scene> m_scenes;

		std::unique_ptr<RHI::Fence>			m_fence_in_flight;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_image_available;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_render_finished;
	};
	

}} // namespace Albedo::Runtime