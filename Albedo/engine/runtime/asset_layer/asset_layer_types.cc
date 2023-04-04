#include "asset_layer_types.h"
#include "image/image_loader.h"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Albedo {
namespace Runtime
{
	Image::~Image()
	{
		stbi_image_free(data);
	}

	std::shared_ptr<Image> LoadImageTask::Execute()
	{
		return ImageLoader::LoadTexture2D(m_image_path);
	}

}} // namespace Albedo::Runtime