#pragma once

#include <AlbedoLog.hpp>
#include <AlbedoPattern.hpp>

#include "UI_widget/UI_widget.h"

namespace Albedo {
namespace Runtime
{

	class UISystem : public pattern::Singleton<UISystem>
	{
		friend class pattern::Singleton<UISystem>;
	public:
		using UIEvent = std::function<void()>;
		void RegisterUIEvent(std::string name, UIEvent event);

	public:


	public: // Create Widgets
		std::shared_ptr<UIWidget::Texture> CreateWidgetTexture(std::shared_ptr<RHI::VMA::Image> image);
	
	public: // Main Scene
		const ImVec2& GetMainSceneSize() const { return main_scene_size; }
		float GetMainSceneAspectRatio() const { return main_scene_size.x / main_scene_size.y; }
		static bool IsFocusingOnMainScene() { return main_scene_is_focused; }

	private:
		//--------------------------------------------------------------------------------------------------------------
		/*Call in Render System*/ friend class RenderSystem;
		//--------------------------------------------------------------------------------------------------------------
		void Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, 
			std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass);
		bool ShouldRender() const { return m_should_render.value(); }
		void Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer);
		std::shared_ptr<UIWidget::Texture>	main_scene;
		inline static bool									main_scene_is_focused = false;
		std::shared_ptr<RHI::Sampler>			main_scene_sampler;
		std::shared_ptr<RHI::VMA::Image>	main_scene_image;
		ImVec2													main_scene_size;
		//--------------------------------------------------------------------------------------------------------------

	private:
		UISystem() = default;
		~UISystem();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		std::unordered_map<std::string, UIEvent> m_ui_events;
		std::optional<bool> m_should_render; // Init after calling Initialize()

	private:
		void create_main_window();
	};

}} // namespace Albedo::Runtime