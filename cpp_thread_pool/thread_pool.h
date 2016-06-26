#pragma once

#include <functional>
#include <queue>
#include <thread>

namespace tp {
	/// Supported priorities
	enum class prio
	{
		LOW = 0,
		MED = 1,
		HIGH = 2
	};

}

template <typename RetType, typename... ArgTypes>
class thread_pool
{
public:
	/// Type of the function that can be handled by this thread_pool
	using f_t = std::function<RetType(ArgTypes...)>;

	/// if num_threads is zero the std::thread::hardware_concurrency value is used
	thread_pool(size_t num_threads = 0);

	/// Adds another task to the queu, prio of 0 is executed last
	void add(f_t f, tp::prio p, ArgTypes... args);

private:

	/// Helper struct that binds all the items we need in the queue
	struct func_data
	{
		f_t f;
		size_t prio;
	};

	/// Comparator used for the priority queue
	std::function<bool(func_data, func_data)> data_cmp;
	

	std::priority_queue<func_data, std::vector<func_data>, decltype(data_cmp)> m_queue;
	std::vector<std::thread> m_threads;
};

template<typename RetType, typename ...ArgTypes>
inline thread_pool<RetType, ArgTypes...>::thread_pool(size_t num_threads)
{

	// Initialize the compare function
	data_cmp = [](func_data a, func_data b) {
		return a.prio > b.prio;
	};

	if (num_threads == 0) 
	{
		num_threads = std::thread::hardware_concurrency();
	}

	for (size_t i = 0; i < num_threads; ++i)
	{
		m_threads.emplace_back({});
	}
}

template<typename RetType, typename ...ArgTypes>
inline void thread_pool<RetType, ArgTypes...>::add(f_t f, tp::prio p, ArgTypes... args)
{
	for (const auto& t : m_threads)
	{
		if (!t.joinable())
		{
			t(std::bind(f, args));
		}
	}

}
