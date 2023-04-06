#pragma once

#include <AlbedoRHI.hpp>

#include <runtime/function_layer/render/camera/camera.h>
#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	class Canvas
	{
		using SID = size_t;
		struct Scene
		{
			struct Node
			{
				
			};
			std::vector<Node> nodes;
			std::shared_ptr<RHI::VMA::Buffer> vertex_buffer;
			std::shared_ptr<RHI::VMA::Buffer> index_buffer;
			std::shared_ptr<RHI::DescriptorSet> descriptor_sets;
			std::shared_ptr<RHI::Sampler> m_texture_sampler;
			std::vector<std::shared_ptr<RHI::VMA::Image>> textures;
		};

	public:
		void Render(SID scene_id, RHI::GraphicsPipeline* pipeline); // Render all Scenes and their nodes (Future: Trim)

		void LoadScene(std::shared_ptr<SModel> scene);

	public:
		Canvas() = delete;
		Canvas(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
			m_vulkan_context{ std::move(vulkan_context) } {}

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::vector<Scene> m_scenes;

		std::shared_ptr<RHI::CommandBuffer> m_command_buffer;

		std::unique_ptr<RHI::Fence>			m_fence_in_flight;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_image_available;
		std::unique_ptr<RHI::Semaphore>	m_semaphore_render_finished;
	};
	

}} // namespace Albedo::Runtime