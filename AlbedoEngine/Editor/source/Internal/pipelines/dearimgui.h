#pragma once

#include <Albedo/Graphics/widgets.h>

namespace Albedo
{
	using namespace Albedo::Graphics;

	class ImGuiPipeline final
		:public GraphicsPipeline
	{
	public:
		virtual void Begin(std::shared_ptr<CommandBuffer> commandbuffer) override;
		virtual void End(std::shared_ptr<CommandBuffer> commandbuffer)	 override;

	public:
		ImGuiPipeline() :GraphicsPipeline{} {}; // Created by ImGui
	};

} // namespace Albedo