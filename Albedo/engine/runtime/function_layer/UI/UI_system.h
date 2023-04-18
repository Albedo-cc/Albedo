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
	public: // Interface
		std::shared_ptr<UIWidget::Texture> CreateWidgetTexture(std::shared_ptr<RHI::VMA::Image> image);

	private:
		friend class RenderSystem; // Call in Render System
		void Initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass);
		bool ShouldRender() const { return m_should_render.value(); }
		void Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer);

	private:
		UISystem() = default;
		~UISystem();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		std::unordered_map<std::string, std::function<void()>> m_draw_calls;
		std::optional<bool> m_should_render; // Init after calling Initialize()
	};

}} // namespace Albedo::Runtime