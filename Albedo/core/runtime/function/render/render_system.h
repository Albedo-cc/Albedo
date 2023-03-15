#pragma once

#include "../window/window_system.h"
#include "RHI/Vulkan/vulkan_manager.h"

#include <memory>

namespace Albedo {
namespace Runtime
{

	class RenderSystem
	{
	public:
		RenderSystem():
			m_window_system {std::make_shared<WindowSystem>()},
			m_vulkan_manager{std::make_shared<RHI::VulkanManager>(m_window_system->GetWindow())}
		{

		}
		void Update()
		{

		}

	private:
		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<RHI::VulkanManager> m_vulkan_manager;
	};

}} // namespace Albedo::Runtime