#pragma once

#include <AlbedoPattern.hpp>
#include "asset_layer_types.h"

namespace Albedo {
namespace Runtime
{

	class AssetManager : public pattern::Singleton<AssetManager>
	{
		friend class pattern::Singleton<AssetManager>;
	public:
		// Load Image Asset
		std::shared_ptr<Image> LoadTexture2D(std::string texture_name);
		std::shared_ptr<LoadImageTask> AsyncLoadTexture2D(std::string texture_name);

		// Load Model Asset

	private:
		AssetManager() = default;

	private: // Path (Future: Load from Platform Layer)
		const std::string& get_asset_path_texture() { static const std::string path = "resource/texture/";  return path; }
	};

}} // namespace Albedo::Runtime