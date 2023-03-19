#pragma once

#include <AlbedoRHI.hpp>

#include "../window/window_system.h"

#include <memory>

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
	public:
		RenderSystem();

		void Update()
		{

		}

	private:
		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that this context will be released at last.

		std::vector<std::unique_ptr<RHI::RenderPass>> m_render_passes;
	};

}} // namespace Albedo::Runtime