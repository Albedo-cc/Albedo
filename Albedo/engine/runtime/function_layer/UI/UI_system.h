#pragma once

#include <AlbedoLog.hpp>
#include <AlbedoPattern.hpp>

#include "UI_widget.h"

namespace Albedo {
namespace Runtime
{

	class UISystem : public pattern::Singleton<UISystem>
	{
		friend class pattern::Singleton<UISystem>;
	public:
		using UIEvent = std::function<void()>;
		void RegisterUIEvent(std::string name, UIEvent event);

	public: // Widgets
		std::shared_ptr<UIWidget::Texture> CreateWidgetTexture(std::shared_ptr<RHI::VMA::Image> image);

	private:
		friend class RenderSystem; // Call in Render System
		void Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass);
		bool ShouldRender() const { return m_should_render.value(); }
		void Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer);
		std::shared_ptr<RHI::VMA::Image> main_scene_image;
		std::shared_ptr<UIWidget::Texture> main_scene;

	private:
		UISystem() = default;
		~UISystem();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		std::unordered_map<std::string, UIEvent> m_ui_events;
		std::optional<bool> m_should_render; // Init after calling Initialize()
	};

}} // namespace Albedo::Runtime