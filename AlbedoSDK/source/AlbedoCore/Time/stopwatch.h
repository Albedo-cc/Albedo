#pragma once

#include "duration.h"

namespace Albedo
{

    class StopWatch
    {
    public:
        void Reset()
        {
            m_current_time = std::chrono::high_resolution_clock::now();
        }

        Duration Split()
        { 
            auto split_time_stamp = std::chrono::high_resolution_clock::now();
            auto time_duration = split_time_stamp - m_current_time;
            return time_duration;
        }

    public:
        StopWatch() :m_current_time{ std::chrono::high_resolution_clock::now()} {};

    private:
        std::chrono::steady_clock::time_point m_current_time;
    };
} // namespace Albedo