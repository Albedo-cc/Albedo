#pragma once
#include <AlbedoTime.hpp>

#include <core/math/math.h>
#include <runtime/asset_layer/asset_manager.h>
#include <runtime/function_layer/window/window_system.h>

#include "scene/model.h"
#include "camera/camera.h"
#include "paint_box/easel.h"

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
		Easel m_easel;

		std::vector<TempModel> m_models;
		std::shared_ptr<RHI::VMA::Image> m_image;

		std::vector<std::shared_ptr<RHI::RenderPass>> m_render_passes;

	private:
		void create_render_passes();
		void handle_window_resize();

		void load_models();
	};

}} // namespace Albedo::Runtime