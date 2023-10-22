#pragma once

#include <Albedo.Graphics>

namespace Albedo{
namespace APP
{
	
	class BackgroundPass final
		:public GRI::RenderPass
	{
	public:
		enum Subpass { Skybox, };

	public:
		BackgroundPass();
		//virtual ~BackgroundPass() noexcept override {};
	};
	
}} // namespace Albedo::APP