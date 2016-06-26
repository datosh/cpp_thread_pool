#pragma once

#include <functional>
#include <thread>
#include <queue>
#include <vector>
#include <chrono>
#include <mutex>

// debugging
#include <iostream>

class sqlcon {
public:
	sqlcon() {};
};

class tp
{
public:

	/// TODO: is sqlcon const? 
	using func_t = std::function<bool(sqlcon&, const std::string&)>;

	enum class prio_e
	{
		DEFAULT = 0,
		LOW, MID, HIGH
	};

	struct func_data_t
	{
		func_t f;
		prio_e prio;
		std::string arg;

		func_data_t() : arg("") {};

		func_data_t(func_t f, const std::string& a, prio_e p) : f(f), prio(p), arg(a) {};

		func_data_t operator=(const tp::func_data_t& other)
		{
			f = other.f;
			prio = other.prio;
			arg = other.arg;
			return *this;
		}
	};

	tp(size_t pool_size);

	void insert(func_t f, const std::string& s, prio_e prio = prio_e::DEFAULT);

	void terminate();

	size_t num_tasks();

	~tp();

private:

	void thread_idle();

	std::mutex m_queue_mutex;

	std::function<bool(func_data_t, func_data_t)> m_comparator = [](tp::func_data_t a, tp::func_data_t b) {return a.prio < b.prio;};
	std::priority_queue<func_data_t, std::vector<func_data_t>, decltype(m_comparator)> m_queue;

	/// Container for threads in pool
	std::vector<std::thread> m_threads;

	bool m_is_running = true;
		
};

inline tp::tp(size_t pool_size) : m_queue(m_comparator)
{
	for (size_t i = 0; i < pool_size; ++i)
	{
		m_threads.emplace_back(&tp::thread_idle, this);
	}
}

inline void tp::insert(func_t f, const std::string& s, prio_e prio)
{
	std::cout << "trying to add..." << std::endl;
	std::flush(std::cout);

	std::lock_guard<std::mutex> guard(m_queue_mutex);

	m_queue.emplace(f, s, prio);
	std::cout << "ADDED!" << std::endl;
	std::flush(std::cout);
}

inline void tp::terminate()
{
	m_is_running = false;

	std::for_each(m_threads.begin(), m_threads.end(), std::mem_fn(&std::thread::join));
}

inline size_t tp::num_tasks()
{
	std::lock_guard<std::mutex> guard(m_queue_mutex);

	return m_queue.size();
}

inline tp::~tp()
{
	terminate();
}

inline void tp::thread_idle()
{
	sqlcon con{};

	func_data_t task;
	// TODO: solution without is_valid
	bool is_valid = false;

	while (m_is_running)
	{
		// lock mutex non-blocking
		{
			std::lock_guard<std::mutex> guard(m_queue_mutex);
			if (!m_queue.empty()) {
				task = m_queue.top();
				m_queue.pop();
				is_valid = true;
			}
		}

		if (is_valid)
		{
			task.f(con, task.arg);
			is_valid = false;
		}
		
		// sleep while waiting for other jobs
		std::this_thread::sleep_for(std::chrono::milliseconds(250));
	}
}
