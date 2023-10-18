#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo
{
	
	class EditorPass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { ImGui, MAX_SUBPASS };

	public:
		EditorPass();
		virtual ~EditorPass() noexcept override {};
	};
	
} // namespace Albedo