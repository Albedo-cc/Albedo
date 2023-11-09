#pragma once

#include <runtime/renderer/renderer.h>

namespace Albedo{
namespace APP
{
	using namespace Albedo::Graphics;
	
	class BackgroundPass final
		:public RenderPass
	{
	public:
		enum Subpass { Skybox, };

	public:
		BackgroundPass();
		//virtual ~BackgroundPass() noexcept override {};
	};
	
}} // namespace Albedo::APP