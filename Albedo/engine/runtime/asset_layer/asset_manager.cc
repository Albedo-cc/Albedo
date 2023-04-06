#include "asset_manager.h"

namespace Albedo {
namespace Runtime
{

	std::shared_ptr<Image> AssetManager::
		LoadTexture2D(std::string texture_name, AssetUsageFlags usage/* = ASSET_USAGE_ALL*/)
	{
		return ImageLoader::LoadTexture2D(get_asset_path_texture() + texture_name);
	}

	std::shared_ptr<ImageFuture> AssetManager::
		AsyncLoadTexture2D(std::string texture_name, AssetUsageFlags usage/* = ASSET_USAGE_ALL*/)
	{
		return std::make_shared<ImageFuture>(get_asset_path_texture() + texture_name);
	}

	std::shared_ptr<SModel> AssetManager::
		LoadModel(std::string model_name, AssetUsageFlags usage)
	{
		return ModelLoader::LoadModel(get_asset_path_model() + model_name, usage);
	}

	std::shared_ptr<ModelFuture> AssetManager::
		AsyncLoadModel(std::string model_name, AssetUsageFlags usage)
	{
		return std::make_shared<ModelFuture>(get_asset_path_model() + model_name, usage);
	}

}} // namespace Albedo::Runtime