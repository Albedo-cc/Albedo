#pragma once
#include <memory>

namespace Albedo{
namespace APP
{

	class Runtime final
	{
		friend class AlbedoAPP;
	public: // User Level Interface
		static void ShutDown(const char* signature);
		static auto IsRunning() -> bool;

	private: // System Level Interface
		static void Initialize();
		static void Terminate();
		static auto Tick() -> bool; // Run

	private:
		static inline bool m_is_running{false};

	private:
		Runtime();
	};

}} // namespace Albedo::APP