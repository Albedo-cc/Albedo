#include "image_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <AlbedoLog.hpp>

namespace Albedo {
namespace Runtime
{

	Image::~Image()
	{
		stbi_image_free(data);
	}

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