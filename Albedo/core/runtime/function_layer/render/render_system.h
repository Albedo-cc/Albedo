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
		RenderSystem() :
			m_window_system{ std::make_shared<WindowSystem>() },
			m_vulkan_context{ std::make_shared<RHI::VulkanContext>(m_window_system->GetWindow()) }
		{

		}

		void Update()
		{

		}

	private:
		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context; // Make sure that this context will be released at last.
	};

}} // namespace Albedo::Runtime