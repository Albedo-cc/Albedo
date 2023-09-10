#pragma once

#include <AlbedoGraphics/GRI.h>

namespace Albedo{
namespace APP
{
	
	class SurfacePass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { Editor, };


	public:
		SurfacePass();
		virtual ~SurfacePass() noexcept override {};
	};
	
}} // namespace Albedo::APP