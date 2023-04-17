#pragma once

#include <AlbedoRHI.hpp>
#include <AlbedoTime.hpp>
#include <AlbedoPattern.hpp>

namespace Albedo {
namespace Runtime
{
	class WindowSystem;
	class RenderSystem;
	class UISystem;

	class RuntimeModule:
		public pattern::Singleton<RuntimeModule>
	{
		friend class pattern::Singleton<RuntimeModule>;
		RuntimeModule();
		~RuntimeModule(){ m_vulkan_context->WaitDeviceIdle(); }

	public:
		void Run();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<WindowSystem> m_window_system;
		std::shared_ptr<RenderSystem> m_render_system;
	};

}} // namespace Albedo::Runtime