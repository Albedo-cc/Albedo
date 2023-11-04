#pragma once

#include <Albedo/Core/Norm/concepts.h>

namespace Albedo { namespace Pattern 
{

	template<typename T>
    class Singleton
    {
    //[USAGE]
    //class SINGLETON : public Singleton<?>
    //{ 
    //  friend class Singleton<SINGLETON>; 
    //  SINGLETON() = default; // Make Constructor private.
    //};
    protected:
        Singleton() = default;
        virtual ~Singleton() noexcept = default;

    public:
        static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value)
        {
            static T singleton;
            return singleton;
        }

    protected:
        Singleton(const Singleton&)             = delete;
        Singleton(Singleton&&)                  = delete;
        Singleton& operator=(const Singleton&)  = delete;
        Singleton& operator=(Singleton&&)       = delete;
    };

    template<typename T>
    concept SingletonType = std::is_class_v<Singleton<T>> || std::is_base_of_v<Singleton<T>, T>;
	
}} // namespace Albedo::Pattern