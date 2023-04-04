#include "image_loader.h"

#include <stb_image.h>

namespace Albedo {
namespace Runtime
{

	std::shared_ptr<Image> ImageLoader::LoadTexture2D(std::string_view image_path)
	{
		auto texture = load(image_path);
		if (texture->width % 4 || texture->height % 4)
		{
			log::warn("Bad 2D Texture Size({}, {})! - https://github.com/gpuweb/gpuweb/issues/363", 
				texture->width, texture->height);
		}
		return texture;
	}

	std::shared_ptr<Image> ImageLoader::load(std::string_view image_path)
	{
		auto image = std::make_shared<Image>();
		image->data = stbi_load(image_path.data(),
												&image->width, &image->height, &image->channel,
												STBI_rgb_alpha);
		if (!image->data) throw std::runtime_error("Failed to load image!");
		return image;
	}

}} // namespace Albedo::Runtime