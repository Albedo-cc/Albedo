#include "asset_manager.h"

#include "image/image_loader.h"

namespace Albedo {
namespace Runtime
{

	std::shared_ptr<Image> AssetManager::
		LoadTexture2D(std::string texture_name)
	{
		auto image = ImageLoader::LoadTexture2D(get_asset_path_texture() + texture_name);
		
		return image;
	}

	std::shared_ptr<LoadImageTask> AssetManager::
		AsyncLoadTexture2D(std::string texture_name)
	{
		return std::make_shared<LoadImageTask>(get_asset_path_texture() + texture_name);
	}

}} // namespace Albedo::Runtime