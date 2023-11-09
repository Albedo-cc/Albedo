#pragma once

#include <runtime/renderer/renderer.h>

namespace Albedo{
namespace APP
{
	
	class GeometryPass final
		:public RenderPass
	{
	public:
		enum Subpass { Triangles, Lines, Points };

	public:
		GeometryPass();
		//virtual ~GeometryPass() noexcept override {};
	};
	
}} // namespace Albedo::APP