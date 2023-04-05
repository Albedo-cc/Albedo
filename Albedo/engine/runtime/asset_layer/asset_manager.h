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
	class LoadImageTask;

	class AssetManager : public pattern::Singleton<AssetManager>
	{
		friend class pattern::Singleton<AssetManager>;
	public:
		// Load Image Asset
		std::shared_ptr<Image> LoadTexture2D(std::string texture_name, AssetUsageFlags usage = ASSET_USAGE_ALL);
		std::shared_ptr<LoadImageTask> AsyncLoadTexture2D(std::string texture_name, AssetUsageFlags usage = ASSET_USAGE_ALL);

		// Load Model Asset
		std::shared_ptr<SModel> LoadModel(std::string model_name, AssetUsageFlags usage);

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


	class LoadImageTask : public Core::AsyncTask<std::shared_ptr<Image>>
	{
	public:
		virtual std::shared_ptr<Image> Execute() override
		{
			return ImageLoader::LoadTexture2D(m_image_path);
		}

	public:
		LoadImageTask(std::string image_path) : m_image_path{ std::move(image_path) } {}

	private:
		std::string m_image_path;
	};


}} // namespace Albedo::Runtime