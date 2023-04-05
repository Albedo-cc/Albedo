#pragma once

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{
	
	class ModelLoader
	{
	public:
		static std::shared_ptr<SModel> LoadModel(std::string_view model_path, AssetUsageFlags usage);

	private:
		static std::shared_ptr<SModel> load_model_glTF(std::string_view model_path, AssetUsageFlags usage);
	};

}} // namespace Albedo::Runtime