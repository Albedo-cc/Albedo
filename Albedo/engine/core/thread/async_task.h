#pragma once

#include <future>
#include <chrono>
#include <limits>
#include <functional>

namespace Albedo{
namespace Core
{

	template<typename Result_T>
	class AsyncTask
	{
	public:
		virtual Result_T Execute() = 0; // Execute automatically in constructor

		virtual Result_T WaitResult(int64_t timeout_ms = std::numeric_limits<int64_t>::max())
			throw (std::runtime_error)
		{
			if (m_future.wait_for(std::chrono::milliseconds(timeout_ms)) == std::future_status::timeout)
				throw std::runtime_error("Albedo Core - Task Timeout!");
			return  m_future.get();
		}
		void GetProgress() const { return m_current_progress; }

	public:
		AsyncTask(std::launch mode = std::launch::async | std::launch::deferred) 
		{ m_future = std::async(mode, &AsyncTask::Execute, this); }
		virtual ~AsyncTask() {}

	protected:
		std::future<Result_T> m_future;
		uint8_t m_current_progress = 0; // 0 ~ 100 is recommended
	};

}} // namespace Albedo::Core