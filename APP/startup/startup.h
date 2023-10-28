#pragma once
#include "config.h"
#include "local.h"
#include "../runtime/runtime.h"
#include "../sandbox/sandbox.h"

// Sort by Initialization Order
#include <Albedo.Core.Log>
#include <Albedo.Graphics>
#include <Albedo.Editor>
#include <Albedo.System.Window>

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

		// Init Editor
		Editor::Initialize(
		{
			.font_path = "C:\\Frozen Zone\\MyGitHub\\Albedo\\APP\\asset\\font\\calibri.ttf",
			.font_size = 16.0,
		});

	}

	void Run()
	{
		Log::Info("{} is running.", APP_NAME);

		Runtime::Initialize();	

		while (Runtime::Tick())
		{
			Sandbox();
		}

		Runtime::Terminate();
	}

	int Terminate() noexcept
	{
		Log::Info("{} is being terminated...", APP_NAME);

		Editor::Terminate();
		GRI::Terminate();
		WindowSystem::Terminate();

		return EXIT_SUCCESS;
	}

}} // namespace Albedo::APP