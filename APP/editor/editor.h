#pragma once

#include <AlbedoSystem/UI/UI_system.h>

namespace Albedo{
namespace APP
{

	class Editor
	{
	public:
		static void Initialize();
        static void Terminate();
        static void Tick(std::shared_ptr<GRI::CommandBuffer> commandbuffer);

	private:
		static inline std::shared_ptr<GRI::Image> sm_main_camera;

	private:
		Editor() = delete;
	};
	
}} // namespace Albedo::APP