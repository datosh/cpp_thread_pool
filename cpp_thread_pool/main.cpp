#include <iostream>
#include <random>


#include "tp.h"

size_t get_random_value(size_t min, size_t max) {

	std::random_device r;
	std::default_random_engine e1(r());
	std::uniform_int_distribution<size_t> uniform_dist(min, max);
	return uniform_dist(e1);
}

void concat(int a, int b) {
	std::cout << a << b << std::endl;
}

bool add_ip(sqlcon connection, const std::string& ip)
{
	std::cout << "add_ip" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}
bool add_agent(sqlcon connection, const std::string& agent)
{
	std::cout << "add_agent" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}
bool add_req(sqlcon connection, const std::string& req)
{
	std::cout << "add_req" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}
bool add_resp(sqlcon connection, const std::string& resp)
{
	std::cout << "add_resp" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}
bool is_ip_in_db(sqlcon connection, const std::string& ip)
{
	std::cout << "is_ip_in_db" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}
bool is_agent_in_db(sqlcon connection, const std::string& agent)
{
	std::cout << "is_agent_in_db" << std::endl;
	std::flush(std::cout);
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	return true;
}

int main(int argc, char** argv) 
{
	tp thread_pool(8);

	std::vector<std::function<bool(sqlcon, const std::string&)>> funcs;
	funcs.push_back(add_ip);
	funcs.push_back(add_req);
	funcs.push_back(add_resp);
	funcs.push_back(add_agent);
	funcs.push_back(is_ip_in_db);
	funcs.push_back(is_ip_in_db);

	std::vector<tp::prio_e> prios{ tp::prio_e::LOW, tp::prio_e::MID, tp::prio_e::HIGH };

	while (true) {

		if (thread_pool.num_tasks() < 20) {

			std::cout << "Tasks: " << thread_pool.num_tasks() << std::endl;
			std::flush(std::cout);

			size_t idx = get_random_value(0, funcs.size() - 1);
			size_t prio_idx = get_random_value(0, prios.size() - 1);
			tp::prio_e prio = prios[prio_idx];
			thread_pool.insert(funcs[idx], "bla", prio);
		}
		
		std::this_thread::sleep_for(
			std::chrono::milliseconds(get_random_value(0, 500))
		);
	};
}