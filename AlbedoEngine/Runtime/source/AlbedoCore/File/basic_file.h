#pragma once

#include <AlbedoCore/Log/log.h>
#include <AlbedoCore/Norm/assert.h>
#include <AlbedoCore/Norm/types.h>

#include <fstream>
#include <string>
#include <string_view>

namespace Albedo
{

	template<typename BufferType>
	class File
	{
	public:
		static constexpr size_t WHOLE_SIZE = 0;

	public:
		virtual void Save() const = 0;

		auto		 GetPath() const -> std::string_view { return m_path; }
		virtual auto GetSize() const -> size_t = 0;

	protected:
		std::string	m_path;
		BufferType	m_buffer;
		EnumBits	m_options;

	public:
		File() = delete;
		File(std::string path) : m_path{ std::move(path) } {};
		virtual ~File() noexcept = default;
	};
   
} // namespace Albedo