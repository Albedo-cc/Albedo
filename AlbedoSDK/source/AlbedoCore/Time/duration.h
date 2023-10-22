#pragma once

#include <chrono>

namespace Albedo
{
    class Duration
    {
    public:
        double nanoseconds()  const { return m_duration.count(); }
        double microseconds() const { return std::chrono::duration_cast<std::chrono::microseconds>(m_duration).count(); }
        double milliseconds() const { return std::chrono::duration_cast<std::chrono::milliseconds>(m_duration).count(); }
        double seconds()      const { return std::chrono::duration_cast<std::chrono::seconds>(m_duration).count(); }
        double minutes()      const { return std::chrono::duration_cast<std::chrono::minutes>(m_duration).count(); }
        double hours()        const { return std::chrono::duration_cast<std::chrono::hours>(m_duration).count(); }
        double days()         const { return std::chrono::duration_cast<std::chrono::days>(m_duration).count(); }
    public:
        Duration() = delete;
        Duration(const std::chrono::steady_clock::duration& time_stamp) :m_duration{ time_stamp } {}
        Duration(std::chrono::steady_clock::duration&& time_stamp) :m_duration{ time_stamp } {}
        Duration& operator=(const std::chrono::steady_clock::duration& time_stamp) { m_duration = time_stamp; }
        Duration& operator=(std::chrono::steady_clock::duration&& time_stamp){ m_duration = time_stamp; }
    private:
        std::chrono::steady_clock::duration m_duration;
    };

} // namespace Albedo