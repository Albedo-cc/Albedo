#pragma once

#include <AlbedoRHI.hpp>

#include "palette.h"
#include "../environment/scene.h"

namespace Albedo {
namespace Runtime
{

	class Canvas // One Frame
	{
		friend class Easel;
	public:
		struct SyncMeta
		{
			std::unique_ptr<RHI::Fence>			fence_in_flight;
			std::unique_ptr<RHI::Semaphore>	semaphore_image_available;
			std::unique_ptr<RHI::Semaphore>	semaphore_render_finished;

			std::unique_ptr<RHI::Semaphore>	semaphore_screenshot_finished;
			std::unique_ptr<RHI::Semaphore>	semaphore_ui_finished;
		};

	public:
		void	Paint(std::shared_ptr<RHI::CommandBuffer> commandBuffer,
			RHI::GraphicsPipeline* brush,
			std::shared_ptr<Scene> scene);

		SyncMeta		syncmeta;
		// Command Buffers
		std::shared_ptr<RHI::CommandBuffer> cmd_buffer_front;
		std::shared_ptr<RHI::CommandBuffer> cmd_buffer_ui;

	public:
		Canvas() = default; // Created and initialized by the Easel

	private:
		void paint_model_node(RHI::GraphicsPipeline* brush, Scene& scene, std::shared_ptr<Model::Node> model_node);
	};
	

}} // namespace Albedo::Runtime