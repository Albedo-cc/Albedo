#pragma once

#include <string>
#include <filesystem>

namespace Albedo { namespace Platform
{
	struct Path
    {
        static inline const std::string WorkDir{ std::filesystem::current_path().string() };
        
        static inline const std::string Asset       { "asset/" };
        static inline const std::string Cache       { "cache/" };
        static inline const std::string Config      { "config/" };
        static inline const std::string Settings    { "settings/" };
    };

}} // namespace Albedo::Platform