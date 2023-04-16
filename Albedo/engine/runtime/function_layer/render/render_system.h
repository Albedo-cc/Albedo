#pragma once
#include <AlbedoRHI.hpp>
#include <AlbedoTime.hpp>

namespace Albedo {
namespace Runtime
{
	class Camera;
	class Easel;
	class Scene;
	class UISystem;

	class RenderSystem
	{
		friend class RuntimeModule;
		enum RenderPasses
		{
			render_pass_forward,
			render_pass_UI,

			MAX_RENDER_PASS_COUNT
		};

	public:
		RenderSystem() = delete;
		RenderSystem(std::shared_ptr<RHI::VulkanContext> vulkan_context);
		~RenderSystem() { m_vulkan_context->WaitDeviceIdle(); }

		void Update();
		void ConnectUISystem(std::shared_ptr<UISystem> UI);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that vulkan context will be released at last.

		std::shared_ptr<Camera> m_camera;
		std::shared_ptr<Easel> m_easel;
		std::shared_ptr<Scene> m_scene;

		std::vector<std::shared_ptr<RHI::RenderPass>> m_render_passes;

		std::weak_ptr<UISystem> wp_system_UI;

	private:
		void create_render_passes();
		void handle_window_resize();
	};

}} // namespace Albedo::Runtime