#pragma once

#include <AlbedoPattern.hpp>

#include "asset_types.h"

#include <string_view>
#include <unordered_map>

namespace Albedo {
namespace Runtime
{

	/*class AssetManager:
		public pattern::Singleton<AssetManager>
	{
		friend class pattern::Singleton<AssetManager>;
	public:
		std::weak_ptr<Image> GetImage(std::string_view image_name);

	public:
		AssetManager();

	private:
		std::unordered_map<std::string, std::shared_ptr<Image>> m_images;
	};*/

}} // namespace Albedo::Runtime