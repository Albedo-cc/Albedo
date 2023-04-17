#include "asset_manager.h"

namespace Albedo {
namespace Runtime
{

	std::shared_ptr<Image> AssetManager::
		LoadTexture2D(std::string texture_name)
	{
		return ImageLoader::LoadTexture2D(get_asset_path_texture() + texture_name);
	}

	std::shared_ptr<ImageFuture> AssetManager::
		AsyncLoadTexture2D(std::string texture_name)
	{
		return std::make_shared<ImageFuture>(get_asset_path_texture() + texture_name);
	}

	std::shared_ptr<Image> AssetManager::
		LoadIcon(std::string icon_name)
	{
		return ImageLoader::LoadTexture2D(get_asset_path_icon() + icon_name);
	}

	std::shared_ptr<ImageFuture> AssetManager::
		AsyncLoadIcon(std::string icon_name)
	{
		return std::make_shared<ImageFuture>(get_asset_path_icon() + icon_name);
	}

	std::shared_ptr<Model> AssetManager::
		LoadModel(std::string model_name)
	{
		return ModelLoader::LoadModel(get_asset_path_model() + model_name);
	}

	std::shared_ptr<ModelFuture> AssetManager::
		AsyncLoadModel(std::string model_name)
	{
		return std::make_shared<ModelFuture>(get_asset_path_model() + model_name);
	}

}} // namespace Albedo::Runtime