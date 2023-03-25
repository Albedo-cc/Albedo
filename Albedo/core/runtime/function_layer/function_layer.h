#pragma once

#include "../runtime_context.h"
#include "render/render_system.h"
#include "window/window_system.h"

namespace Albedo {
namespace Runtime
{

	class FunctionLayer final:
		public RuntimeLayer
	{
	public:
		virtual void Update() override
		{	
			//std::this_thread::sleep_for(std::chrono::milliseconds(200));

			if (m_window_system->ShouldClose()) 
				m_runtime_context.Stop("Albedo Runtime - Function Layer: WindowSystem::ShouldClose()");
			else  m_window_system->Update(); 

			m_render_system->Update();
		}

		FunctionLayer() = delete;
		FunctionLayer(RuntimeContext& runtime_context) :
			RuntimeLayer{ runtime_context },
			m_window_system{std::make_shared<WindowSystem>()},
			m_render_system{ std::make_unique<RenderSystem>(m_window_system) }
		{

		}

	private:
		std::shared_ptr<WindowSystem> m_window_system;
		std::unique_ptr<RenderSystem> m_render_system;
	};

}} // namespace Albedo::Runtime