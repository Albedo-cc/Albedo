#pragma once
#include <memory>

namespace Albedo{
namespace APP
{

	class Runtime final
	{
	public: // User Level Interface
		static auto Tick() -> bool; // Run
		static void ShutDown(const char* signature);
		static auto IsRunning() -> bool;

	public: // System Level Interface
		static void Initialize();
		static void Terminate();

	private:
		static inline bool m_is_running{false};

	private:
		Runtime();
	};

}} // namespace Albedo::APP