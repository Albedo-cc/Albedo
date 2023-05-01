#include "runtime.h"

#include "function_layer/render/render_system.h"
#include "function_layer/window/window_system.h"
#include "function_layer/UI/UI_system.h"
#include "function_layer/control/control_system.h"

#include <net/net.h>
#include "function_layer/render/camera/camera.h"

namespace Albedo {
namespace Runtime
{

	RuntimeModule::RuntimeModule() : 
		m_window_system{ std::make_shared<WindowSystem>() }
	{
		m_vulkan_context = RHI::VulkanContext::Create(m_window_system->GetWindow());
		ControlSystem::instance().Initialize(m_window_system); 
		WindowSystem::SetFramebufferResizeCallback([this]() { m_render_system->handle_window_resize(); });
		
		// Must init Render System after Control System (GLFW callbacks)
		m_render_system = std::make_shared<RenderSystem>(m_vulkan_context); 
	}

	void RuntimeModule::Run()
	{
		static bool RUNNING = false;
		if (RUNNING) log::warn("You tried to run Runtime Module again!");
		RUNNING = true;

		while (!m_window_system->ShouldClose())
		{
			m_window_system->Update();
			ControlSystem::instance().Update();
			m_render_system->Update();

			SyncToServer();
		}
	}

	void RuntimeModule::SyncToServer()
	{
		static auto& net = Net::NetModule::instance();
		if (!net.IsOnline()) return;

		static constexpr float SYNC_PERIOD_MS = 1000.0f / 61.0; // 60FPS
		static time::StopWatch<float> timer;

		static float period = 0.0f;
		period = timer.split().milliseconds();

		if (SYNC_PERIOD_MS < period) // Sync
		{
			period = timer.split(true).days(); // assert == 0

			// Protocbuf will auto-free when call add_allocated_X()
			ABDChamber::Buffer* buffer = new ABDChamber::Buffer();

			auto& camera_matrics = m_render_system->m_camera->GetCameraMatrics();
			const float* ele = camera_matrics.matrix_view.data();

			for (int i = 0; i < 16; ++i, ++ele)
			{
				buffer->add_content(*ele);
			}

			auto view_viewmat = buffer->add_views();
			view_viewmat->set_offset(0);
			view_viewmat->set_size(sizeof(Matrix4f));
			net.SyncCamera(buffer);
		}
	}

}} // namespace Albedo::Runtime