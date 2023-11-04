#pragma once

#include <string>

namespace Albedo
{
	//[Index]
	class Image;

	class Image final
	{
	public:
		enum Type { PNG, JEPG, BMP, KTX }; // Jump Table
		Image(Type type, const std::string& path);
		void Free() noexcept;

		auto GetWidth()			const -> int	{ return m_width; }
		auto GetHeight()		const -> int	{ return m_height; }
		auto GetAspectRatio()	const -> double { return double(m_width) / m_height; }
		auto GetChannels()		const -> int	{ return m_channels; }
		auto GetType()			const -> Type	{ return m_type; }


		~Image() noexcept { Free(); }
		Image() = delete;

	private:
		Type  m_type;
		int   m_width{0}, m_height{0}, m_channels{0};
		void* m_data = nullptr;
	};

} // namespace Albedo