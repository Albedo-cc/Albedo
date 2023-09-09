#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo{
namespace APP
{
	
	class GeometryPass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { Opaque, };

	public:
		GeometryPass();
		virtual ~GeometryPass() noexcept override {};
	};
	
}} // namespace Albedo::APP