#pragma once

#include <AlbedoPattern.hpp>

#include <core/thread/async_task.h>

#include "asset_layer_types.h"
#include "image/image_loader.h"
#include "model/model_loader.h"

namespace Albedo {
namespace Runtime
{
	// Task Types
	class ImageFuture;
	class ModelFuture;

	class AssetManager : public pattern::Singleton<AssetManager>
	{
		friend class pattern::Singleton<AssetManager>;
	public:
		// Load Image Asset
		std::shared_ptr<Image> LoadTexture2D(std::string texture_name, AssetUsageFlags usage = ASSET_USAGE_ALL);
		std::shared_ptr<ImageFuture> AsyncLoadTexture2D(std::string texture_name, AssetUsageFlags usage = ASSET_USAGE_ALL);

		// Load Model Asset
		std::shared_ptr<SModel> LoadModel(std::string model_name, AssetUsageFlags usage);
		std::shared_ptr<ModelFuture> AsyncLoadModel(std::string model_name, AssetUsageFlags usage);

	public :
		// Keep Assets
		/*void RegisterImage(std::shared_ptr<Image> image);
		void RegisterModel(std::shared_ptr<Model> image);*/

	private:
		AssetManager() = default;

	private: // Path (Future: Load from Platform Layer)
		const std::string& get_asset_path_texture() { static const std::string path = "resource/texture/";  return path; }
		const std::string& get_asset_path_model() { static const std::string path = "resource/model/";  return path; }
	};


	class ImageFuture : public Core::AsyncTask<std::shared_ptr<Image>>
	{
	public:
		virtual std::shared_ptr<Image> Execute() override
		{
			return ImageLoader::LoadTexture2D(m_image_path);
		}

	public:
		ImageFuture() = delete;
		ImageFuture(std::string image_path) : m_image_path{ std::move(image_path) } {}

	private:
		std::string m_image_path;
	};

	class ModelFuture : public Core::AsyncTask<std::shared_ptr<SModel>>
	{
	public:
		virtual std::shared_ptr<SModel> Execute() override
		{
			return ModelLoader::LoadModel(m_model_path, m_usage);
		}

	public:
		ModelFuture() = delete;
		ModelFuture(std::string model_path, AssetUsageFlags usage) :
			m_model_path{ std::move(model_path) }, m_usage{ usage } {}

	private:
		std::string m_model_path;
		AssetUsageFlags m_usage;
	};

}} // namespace Albedo::Runtime