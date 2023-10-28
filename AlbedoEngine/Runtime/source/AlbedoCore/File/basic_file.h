#include <AlbedoCore/Log/log.h>
#include <AlbedoCore/Norm/assert.h>
#include <AlbedoCore/Norm/concepts.h>
#include <AlbedoCore/Norm/types.h>

#include <fstream>
#include <string_view>

namespace Albedo
{

	template<Pointer BufferType>
	class File
	{
	public:
		virtual void Read(std::string_view path) const throw(std::runtime_error) = 0;
		virtual void Write() const throw(std::runtime_error) = 0;
		virtual void Save(std::string_view path = nullptr) const throw(std::runtime_error) = 0;

	protected:
		BufferType m_buffer;

	public:
		File() = default;
		virtual ~File() noexcept = default;
	};
   
} // namespace Albedo