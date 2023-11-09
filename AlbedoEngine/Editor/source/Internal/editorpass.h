#pragma once

#include <Albedo/Graphics/widgets.h>

namespace Albedo
{
	using namespace Albedo::Graphics;

	class EditorPass final
		:public RenderPass
	{
	public:
		enum Subpass { ImGui };

	public:
		EditorPass();
		virtual ~EditorPass() noexcept override {};
	};
	
} // namespace Albedo