//#include "asset_manager.h"
//
//#include "image/image_loader.h"
//
//#include <stdexcept>
//
//namespace Albedo {
//namespace Runtime
//{
//
//	AssetManager::AssetManager()
//	{
//		
//	}
//
//	std::weak_ptr<Image> AssetManager::GetImage(std::string_view image_name)
//	{
//		auto result = m_images.find(image_name.data());
//		if (result != m_images.end())
//		{
//			return result->second;
//		}
//		else throw std::runtime_error("Failed")
//	}
//
//}} // namespace Albedo::Runtime