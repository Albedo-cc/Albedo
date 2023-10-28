#include "basic_file.h"

namespace Albedo
{

	class BinaryFile : public File<std::vector<Byte>>
	{
	public:
		virtual void Read(std::string_view path) const throw(std::runtime_error)override
		{

		}
		virtual void Write() const throw(std::runtime_error) override
		{

		}
		virtual void Save(std::string_view path = nullptr) const throw(std::runtime_error) override
		{

		}
	};
   
} // namespace Albedo