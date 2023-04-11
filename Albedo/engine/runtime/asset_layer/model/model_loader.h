#pragma once

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{

	class ModelLoader
	{
	public:
		static std::shared_ptr<Model> LoadModel(std::string_view model_path);

	private:
		static std::shared_ptr<Model> load_model_glTF(std::string_view model_path);
	};

}} // namespace Albedo::Runtime