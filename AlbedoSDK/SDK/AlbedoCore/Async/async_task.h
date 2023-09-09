#include <AlbedoCore/Log/log.h>

#include <future>
#include <functional>

namespace Albedo
{

    template<typename Result_T>
    class AsyncTask
    {
    public:
        Result_T Wait(int64_t timeout_ms = 10000 /*10Sec*/)
        {
            if (std::future_status::timeout == m_future.wait_for(std::chrono::milliseconds(timeout_ms)))
                Log::Error("Async Task Timeout!");
            return m_future.get();
        }

    public:
        AsyncTask() = delete;
        AsyncTask(std::function<Result_T()> task,
            std::launch mode = std::launch::async | std::launch::deferred)
            :
            m_task{std::move(task)},
            m_future{std::async(mode,[this]()->Result_T
            {
                assert(m_task != nullptr);
                return m_task();
            })}
        {/*Empty*/ }

    private:
        std::function<Result_T()> m_task;
        std::future<Result_T> m_future;
    };
   
} // namespace Albedo