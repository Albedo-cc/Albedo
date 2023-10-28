#pragma once
#include <global/config.h>
#include <runtime/runtime.h>
#include <sandbox/sandbox.h>
#include "local.h"

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
		auto& CONFIG = APPConfig::GetView(); // Init
		Log::Info("{} is being started.", CONFIG.app.name);
		
		// Init Window
		WindowSystem::Initialize(WindowSystem::CreateInfo
			{
			.title	  = CONFIG.app.name.data(),
			.width	  = CONFIG.window.width,
			.height	  = CONFIG.window.height,
			.maximize = CONFIG.window.options.maximize,
		});

		// Init GRI
		GRI::Initialize(GRICreateInfo
		{
			.app_name     = CONFIG.app.name.data(),
			.app_window   = WindowSystem::GetWindow(),
			.msg_callback = messenger_callback,
		});

		// Init Editor
		Editor::Initialize(
		{
			.layout    = CONFIG.editor.layout.data(),
			.font_path = CONFIG.editor.font.name.data(),
			.font_size = CONFIG.editor.font.size,
		});

	}

	void Run()
	{
		Log::Info("{} is running.", APPConfig::GetView().app.name);

		Runtime::Initialize();	

		while (Runtime::Tick())
		{
			Sandbox();
		}

		Runtime::Terminate();
	}

	int Terminate() noexcept
	{
		Log::Info("{} is being terminated...", APPConfig::GetView().app.name);

		Editor::Terminate();
		GRI::Terminate();
		WindowSystem::Terminate();

		return EXIT_SUCCESS;
	}

}} // namespace Albedo::APP