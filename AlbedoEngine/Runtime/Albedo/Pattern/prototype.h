#pragma once

namespace Albedo { namespace Pattern 
{

	template<typename T>
    struct Prototype
    {
        virtual auto Clone() -> T = 0; // Recommend Shallow Copy
    };

    template<typename T>
    concept PrototypeType = std::is_base_of_v<Prototype<T>, T>;
	
}} // namespace Albedo::Pattern