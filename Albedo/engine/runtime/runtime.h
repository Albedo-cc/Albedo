#pragma once

#include <AlbedoTime.hpp>

#include "runtime_context.h"
#include "function_layer/function_layer.h"

namespace Albedo {
namespace Runtime
{

	class RuntimeModule:
		public pattern::Singleton<RuntimeModule>
	{
		friend class pattern::Singleton<RuntimeModule>;
		RuntimeModule() 
		{ 
			m_runtime_context.m_is_running = true;  // Startup Context
			// Register Layers
			m_runtime_layers.emplace_back(std::make_unique<FunctionLayer>(m_runtime_context));
		}
	
	public:
		void Run()
		{
			while (m_runtime_context.IsRunning())
			{
				for (auto& runtime_layer : m_runtime_layers)
					runtime_layer->Update();
			}
		}

	private:
		RuntimeContext m_runtime_context;
		std::vector<std::unique_ptr<RuntimeLayer>> m_runtime_layers;
	};

}} // namespace Albedo::Runtime