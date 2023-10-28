#pragma once

#include "basic_file.h"

#include <vector>

namespace Albedo
{

	class BinaryFile : public File<std::vector<Byte>>
	{
	public:
		BinaryFile(std::string path) : File<std::vector<Byte>>(std::move(path))
		{
			std::ifstream file(m_path, std::ios::ate | std::ios::binary);
			if (!file.is_open()) Log::Fatal("Failed to open the binary file {}!", m_path);

			size_t file_size = static_cast<size_t>(file.tellg());
			m_buffer.resize(file_size); 

			file.seekg(0);
			file.read(m_buffer.data(), file_size);

			file.close();
		}

		virtual void Save() const override
		{
			std::ofstream file(m_path);
			if (!file.is_open()) Log::Fatal("Failed to open the binary file {}!", m_path);

			file.write(m_buffer.data(), m_buffer.size());

			file.close();
		}

		virtual size_t GetSize() const override
		{
			return m_buffer.size();
		}
	};
   
} // namespace Albedo