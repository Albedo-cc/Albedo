#pragma once

namespace Albedo{
namespace APP
{

	class Editor
	{
	public:
		static void Initialize();
        static void Destroy();
        static void Tick();

	private:
		Editor() = delete;
	};
	
}} // namespace Albedo::APP