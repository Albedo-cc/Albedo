#pragma once

#include <list>
#include <vector>
#include <memory>
#include <concepts>

namespace Albedo
{

	class FrameObject
	{
		friend class GRI;
	public:
		virtual void Create() = 0;
		virtual void Destroy() noexcept = 0;
	};

	template <typename T>
	concept IsFrameObject = std::is_base_of<FrameObject, T>::value;

	template<IsFrameObject T>
	class FrameObjectManager final
		:public std::enable_shared_from_this<FrameObjectManager<T>>
	{
		friend class GRI;
	public:
		auto Get() -> T&;

	public:
		static inline auto Create()
		{ return std::make_shared<FrameObjectManager<T>>(); }
		FrameObjectManager();

	private:
		std::vector<T> m_frameobjects;
	};

} // namespace Albedo