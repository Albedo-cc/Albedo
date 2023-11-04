#pragma once

#include <Albedo/Graphics/GRI.h>

namespace Albedo
{
	
	class EditorPass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { ImGui };

	public:
		EditorPass();
		virtual ~EditorPass() noexcept override {};
	};
	
} // namespace Albedo