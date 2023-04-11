#pragma once
#include <AlbedoTime.hpp>

#include "render_system_context.h"

#include "model/model.h"
#include "camera/camera.h"
#include <core/math/math.h>
#include <runtime/asset_layer/asset_manager.h>
#include <runtime/function_layer/window/window_system.h>

#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
		enum RenderPasses
		{
			render_pass_forward,

			MAX_RENDER_PASS_COUNT
		};
	public:
		RenderSystem() = delete;
		RenderSystem(std::weak_ptr<WindowSystem> window_system);
		~RenderSystem() { m_vulkan_context->WaitDeviceIdle(); }

		void Update();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that vulkan context will be released at last.
		std::weak_ptr<WindowSystem> m_window_system;
		Camera m_camera;

		std::vector<Model> m_models;
		std::shared_ptr<RHI::VMA::Image> m_image;

		std::vector<std::unique_ptr<RHI::RenderPass>> m_render_passes;

	private:
		void wait_for_next_image_index(FrameState& current_frame_state);

		void create_render_passes();
		void handle_window_resize();

		std::vector<std::shared_ptr<ImageFuture>> begin_loading_images();
		void end_loading_images(std::vector<std::shared_ptr<ImageFuture>> image_tasks);

		void load_models();
	};

}} // namespace Albedo::Runtime