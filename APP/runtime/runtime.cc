#include "runtime.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.Time>
#include <Albedo.Graphics>
#include <Albedo.System.Control>
#include <Albedo.System.Window>

#include "render/renderer.h"

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

		runtime_timer.Reset();
		m_is_running = true;
	}

	void
	Runtime::
	Terminate()
	{
		Log::Info("Runtime Duration: {} s", runtime_timer.Split().seconds());

		Renderer::Destroy();
	}

}} // namespace Albedo::APP