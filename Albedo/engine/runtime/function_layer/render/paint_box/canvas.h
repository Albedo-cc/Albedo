#pragma once

#include <AlbedoRHI.hpp>

#include "palette.h"
#include "scene.h"

namespace Albedo {
namespace Runtime
{

	class Canvas // One Frame
	{
		friend class Easel;
	public:
		void	BeginPainting(std::shared_ptr<RHI::RenderPass> renderPass);
		void	Paint(RHI::GraphicsPipeline* brush, std::shared_ptr<Scene> scene);
		void	EndPainting(std::shared_ptr<RHI::RenderPass> renderPass);

		Palette& GetPalette() { return palette; }

	public:
		Canvas() = default; // Created and initialized by the Easel

	private:
		std::weak_ptr<Scene> last_scene;
		std::shared_ptr<RHI::CommandBuffer> command_buffer;
		Palette palette;

		struct SyncMeta
		{
			std::unique_ptr<RHI::Fence>			fence_in_flight;
			std::unique_ptr<RHI::Semaphore>	semaphore_image_available;
			std::unique_ptr<RHI::Semaphore>	semaphore_render_finished;
		};
		SyncMeta syncmeta; // Used by Easel

	private:
		void paint_model_node(RHI::GraphicsPipeline* brush, Scene& scene, std::shared_ptr<Model::Node> model_node);
	};
	

}} // namespace Albedo::Runtime