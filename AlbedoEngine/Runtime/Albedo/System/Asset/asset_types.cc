#include "asset_types.h"

#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Platform/path.h>

#include <ktx.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Albedo
{
	Image::
	Image(Type type, const std::string& path) :
		m_type{ type }
	{
		switch (m_type)
		{
		case Type::PNG:
		{
			//break;
		}
		case Type::JEPG:
		{
			//break;
		}
		case Type::BMP:
		{
			m_data = stbi_load(path.c_str(), &m_width, &m_height, &m_channels, 0);
			if (!m_data) Log::Error("Failed to load image {}", path);
			break;
		}
		case Type::KTX:
		{
			ktxTexture* ktx_data{};
			if (ktxTexture_CreateFromNamedFile( // [TODO]: Move to CPU end.
				path.c_str(),
				KTX_TEXTURE_CREATE_NO_FLAGS,
				&ktx_data) != KTX_SUCCESS)
				Log::Error("Failed to create KTX Texture from {}.", path);

			m_data = ktx_data;
			break;
		}
		default: ALBEDO_UNEXPECTED_ASSERT;
		}
	}

	void
	Image::
	Free() noexcept
	{
		if (!m_data) return;

		switch (m_type)
		{
		case Type::PNG:
		{
			//break;
		}
		case Type::JEPG:
		{
			//break;
		}
		case Type::BMP:
		{
			stbi_image_free(m_data);
			break;
		}
		case Type::KTX:
		{
			ktxTexture_Destroy(static_cast<ktxTexture*>(m_data));
			break;
		}
		default: ALBEDO_UNEXPECTED_ASSERT;
		}

		m_data = nullptr;
	}
} // namespace Albedo