#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo
{

	
	class ImGuiPipeline final
		:public GRI::GraphicsPipeline
	{
	public:
		virtual void Begin(std::shared_ptr<GRI::CommandBuffer> commandbuffer) override;
		virtual void End(std::shared_ptr<GRI::CommandBuffer> commandbuffer)	 override;

	public:
		ImGuiPipeline() :GraphicsPipeline{} {}; // Created by ImGui
	};

} // namespace Albedo