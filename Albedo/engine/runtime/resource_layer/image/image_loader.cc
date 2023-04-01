#include "image_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <stdexcept>

namespace Albedo {
namespace Runtime
{
	Image::~Image()
	{
		stbi_image_free(data);
	}

	std::shared_ptr<Image> ImageLoader::Load(const char* image_path)
	{
		auto image = std::make_shared<Image>();
		image->data = stbi_load(image_path,
												&image->width, &image->height, &image->channel,
												STBI_rgb_alpha);
		if (!image->data) throw std::runtime_error("Failed to load image!");
		if (image->width % 4 || image->height % 4)
			throw std::runtime_error("Invalid Texture Format - https://github.com/gpuweb/gpuweb/issues/363");
		return image;
	}

}} // namespace Albedo::Runtime