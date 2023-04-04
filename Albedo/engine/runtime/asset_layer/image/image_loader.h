#pragma once

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{

	class ImageLoader
	{
	public:
		static std::shared_ptr<Image> LoadTexture2D(std::string_view image_path);

	private:
		static std::shared_ptr<Image> load(std::string_view image_path);
	};

}} // namespace Albedo::Runtime