#pragma once

#include <memory>

namespace Albedo {
namespace Runtime
{

	struct Image
	{
		int width = 0;
		int height = 0;
		int channel = 0;
		unsigned char* data = nullptr;

		size_t Size() { return static_cast<size_t>(width) * height * channel; }

		Image() = default;
		~Image();
	};

	class ImageLoader
	{
	public:
		static std::shared_ptr<Image> Load(const char* image_path);
	};

}} // namespace Albedo::Runtime