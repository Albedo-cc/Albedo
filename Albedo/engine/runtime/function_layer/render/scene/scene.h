#pragma once

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{

	class Scene
	{
	public:
		struct Node
		{
			std::shared_ptr<Model> model;
		};
	};

}} // namespace Albedo::Runtime