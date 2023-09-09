#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo{
namespace APP
{
	
	class SurfacePass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { UI, };

	public:
		SurfacePass();
		virtual ~SurfacePass() noexcept override {};
	};
	
}} // namespace Albedo::APP