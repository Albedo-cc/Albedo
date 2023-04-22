#pragma once

#include <AlbedoPattern.hpp>

#include <core/thread/async_task.h>

#include "asset_types.h"
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
		std::shared_ptr<Image> LoadTexture2D(std::string texture_name);
		std::shared_ptr<ImageFuture> AsyncLoadTexture2D(std::string texture_name);
		std::shared_ptr<Image> LoadIcon(std::string icon_name);
		std::shared_ptr<ImageFuture> AsyncLoadIcon(std::string icon_name);

		// Load TempModel Asset
		std::shared_ptr<Model> LoadModel(std::string model_name);
		std::shared_ptr<ModelFuture> AsyncLoadModel(std::string model_name);

	public :
		// Keep Assets
		/*void RegisterImage(std::shared_ptr<Image> image);
		void RegisterModel(std::shared_ptr<TempModel> image);*/

	private:
		AssetManager() = default;

	public: // Path (Future: Load from Platform Layer)
		static const std::string& get_asset_path_texture() { static const std::string path = "resource/texture/";  return path; }
		static const std::string& get_asset_path_icon() { static const std::string path = "resource/icon/";  return path; }
		static const std::string& get_asset_path_model() { static const std::string path = "resource/model/";  return path; }
		static const std::string& get_asset_path_shader() { static const std::string path = "resource/shader/";  return path; }
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

	class ModelFuture : public Core::AsyncTask<std::shared_ptr<Model>>
	{
	public:
		virtual std::shared_ptr<Model> Execute() override
		{
			return ModelLoader::LoadModel(m_model_path);
		}

	public:
		ModelFuture() = delete;
		ModelFuture(std::string model_path) :
			m_model_path{ std::move(model_path) } {}

	private:
		std::string m_model_path;
	};

}} // namespace Albedo::Runtime