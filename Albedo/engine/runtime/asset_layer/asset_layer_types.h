#pragma once
#include <AlbedoLog.hpp>

#include <memory>
#include <stdexcept>
#include <string_view>

#include <core/thread/async_task.h>

namespace Albedo {
namespace Runtime
{
	// Asset Types
	struct Image;
	struct Model;

	// Task Types
	class LoadImageTask;

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

	class LoadImageTask : public Core::AsyncTask<std::shared_ptr<Image>>
	{
	public:
		virtual std::shared_ptr<Image> Execute() override;

	public:
		LoadImageTask(std::string image_path) : m_image_path{ std::move(image_path) } {}

	private:
		std::string m_image_path;
	};


}} // namespace Albedo::Runtime