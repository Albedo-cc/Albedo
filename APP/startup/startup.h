#pragma once
#include "config.h"
#include "local.h"
#include "../runtime/runtime.h"
#include "../editor/editor.h"
#include "../sandbox/sandbox.h"

// Sort by Initialization Order
#include <AlbedoCore/Log/log.h>
#include <AlbedoGraphics/GRI.h>
#include <AlbedoSystem/Window/window_system.h>

namespace Albedo{
namespace APP
{

	void StartUp(int argc, char* argv[])
	{
		Log::Info("{} is being started.", APP_NAME);

		// Init Window
		WindowSystem::Initialize(WindowSystem::CreateInfo
		{
			.title    = APP_NAME,
			.width	  = 800,
			.height	  = 600,
			.maximize = false,
		});

		// Init GRI
		GRI::Initialize(GRICreateInfo
		{
			.app_name     = APP_NAME,
			.app_window   = WindowSystem::GetWindow(),
			.msg_callback = messenger_callback,
		});
	}

	void Run()
	{
		Log::Info("{} is running.", APP_NAME);

		Runtime::Initialize();	
		Editor::Initialize();

		while (Runtime::Tick())
		{
			Sandbox();
		}

		Runtime::Terminate();
		Editor::Terminate();
	}

	int Terminate() noexcept
	{
		Log::Info("{} is being terminated...", APP_NAME);

		GRI::Terminate();
		WindowSystem::Terminate();

		return EXIT_SUCCESS;
	}

}} // namespace Albedo::APP