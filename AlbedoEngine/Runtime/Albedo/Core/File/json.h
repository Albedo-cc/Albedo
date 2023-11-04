#pragma once

#include "basic_file.h"

#include <nlohmann/json.hpp>

namespace Albedo
{

	class JSONFile : public File<nlohmann::json>
	{
	public:
		auto operator[](const char* key) const { return m_buffer[key]; }

		virtual void Save() const override
		{
			std::ofstream file(m_path);
			if (!file.is_open()) Log::Fatal("Failed to open the json file {}!", m_path);

			file << m_buffer.dump();

			file.close();
		}

		virtual size_t GetSize() const override
		{
			return m_buffer.size();
		}

	public:
		JSONFile(std::string path) : File<nlohmann::json>{ std::move(path) }
		{
			std::ifstream file(m_path);
			file >> m_buffer;
		}
	};
   
} // namespace Albedo