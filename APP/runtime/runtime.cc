#include "runtime.h"

#include <AlbedoCore/Log/log.h>
#include <AlbedoGraphics/GRI.h>
#include <AlbedoSystem/Control/control_system.h>
#include <AlbedoSystem/Window/window_system.h>
#include <AlbedoSystem/UI/UI_system.h>
#include <AlbedoUtils/time.h>

#include "render/renderer.h"
#include "editor/editor.h"

namespace Albedo{
namespace APP
{
	StopWatch runtime_timer{};

	bool
	Runtime::
	Tick()
	{
		WindowSystem::PollEvents();
		ControlSystem::Process();

		Renderer::Tick();

		return IsRunning();
	}

	void
	Runtime::
	ShutDown(const char* signature) 
	{
		m_is_running = false;
		Log::Warn("Runtime is shutdowned by {}!", signature);
	}

	bool
	Runtime::
	IsRunning()
	{
		return m_is_running && WindowSystem::IsDisplaying();
	}

	void
	Runtime::
	Initialize()
	{
		Renderer::Initialize();

		auto ui_renderpass = Renderer::SearchRenderPass("Surface");
		auto ui_subpass    = ui_renderpass->SeachSubpass("Surface::UI");
		auto ui_descriptor_pool = GRI::GetGlobalDescriptorPool();
		UISystem::Initialize(UISystem::CreateInfo
		{
			.renderpass = *ui_renderpass,
			.subpass	= ui_subpass,
			.descriptor_pool = *ui_descriptor_pool,
			.font_path	= "C:\\Frozen Zone\\MyGitHub\\Albedo\\APP\\asset\\fonts\\calibri.ttf",
			.font_size	= 16.0f,
		});

		Editor::Initialize();

		runtime_timer.Reset();
		m_is_running = true;
	}

	void
	Runtime::
	Terminate()
	{
		Log::Info("Runtime Duration: {} s", runtime_timer.Split().seconds());
		Renderer::Destroy();
		UISystem::Terminate();
		Editor::Destroy();
	}

}} // namespace Albedo::APP