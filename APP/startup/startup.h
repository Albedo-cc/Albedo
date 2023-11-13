#pragma once
#include <global/config.h>
#include <runtime/runtime.h>
#include <sandbox/sandbox.h>
#include "local.h"

// Sort by Initialization Order
#include <Albedo.Core.Log>
#include <Albedo.Platform>
#include <Albedo.Editor>
#include <Albedo.Graphics.RHI>
#include <Albedo.System.Window>
#include <Albedo.System.Control>

namespace Albedo{
namespace APP
{

	class AlbedoAPP
	{
	public:
		static void StartUp(static int argc, char* argv[])
		{
			auto& CONFIG = APPConfig::GetView(); // Init
			Log::Info("{} is being started in {}.", CONFIG.app.name, Platform::Path::WorkDir);
		
			// Init WindowSystem
			WindowSystem::Initialize(WindowSystem::CreateInfo
			{
				.title	  = CONFIG.app.name.data(),
				.width	  = CONFIG.window.width,
				.height	  = CONFIG.window.height,
				.maximize = CONFIG.window.options.maximize,
			});

			// Init ControlSystem
			ControlSystem::Initialize(ControlSystem::CreateInfo
			{
				.enable_keyboard	 = true,
				.enable_mouse_cursor = true,
				.enable_mouse_scroll = true,
			});

			// Init GRI
			RHI::Initialize(RHICreateInfo
			{
				.app_name     = CONFIG.app.name.data(),
				.app_window   = WindowSystem::GetWindow(),
				.msg_callback = messenger_callback,
			});

			// Init Editor
			Editor::Initialize(
			{
				.layout    = Platform::Path::Config + CONFIG.editor.layout,
				.font	   = Platform::Path::Asset  + "font/" + CONFIG.editor.font.name,
				.font_size = CONFIG.editor.font.size,
			});

		}

		static void Run()
		{
			Log::Info("{} is running.", APPConfig::GetView().app.name);

			Runtime::Initialize();

			PrepareSandbox();

			while (Runtime::Tick())
			{
				WindowSystem::Process();
				ControlSystem::Process();

				Sandbox();
			}

			Runtime::Terminate();
		}

		static int Terminate() noexcept
		{
			Log::Info("{} is being terminated...", APPConfig::GetView().app.name);

			Editor::Terminate();
			RHI::Terminate();
			ControlSystem::Terminate();
			WindowSystem::Terminate();

			return EXIT_SUCCESS;
		}

	private:
		AlbedoAPP() = delete;
	};


}} // namespace Albedo::APP