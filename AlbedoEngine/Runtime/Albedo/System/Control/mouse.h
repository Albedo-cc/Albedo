#pragma once

namespace Albedo
{

	class Mouse
	{
	public:
		using Button = unsigned int;
		static Button Left, Middle, Right;

	private:
		Mouse() = delete;
	};

} // namespace Albedo