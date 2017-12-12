#include <iostream>
#include <ctime>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <mutex>


class Noncopyable
{
public:
	// The deletion of the Noncopyable copy constructor makes the
	// declaration of at least one other constructor mandatory in this
	// class, otherwise it becomes impossible to instantiate the
	// children.
	Noncopyable(const Noncopyable&) = delete;
	Noncopyable& operator=(const Noncopyable&) = delete;

protected:

	// However, we do not want the programs instantiate Noncopyable
	// directly which oblige us to declare them protected, therefore
	// accessible to children but not to the outside world.
	constexpr Noncopyable() = default;
	~Noncopyable() = default;
};


class Singleton: Noncopyable
{
	Singleton() {}

public:
	static Singleton &get() {
		// The static variable initialization is thread safe in
		// C++11. According to the standard, §6.7 [stmt.dcl] p4:
		//
		// `If control enters the declaration concurrently while the
		// variable is being initialized, the concurrent execution shall
		// wait for completion of the initialization.`
		//
		static Singleton singleton;
		return singleton;
	}

	static void log(std::string info) {
		auto t0 = std::chrono::system_clock::now();
		std::time_t t0_time = std::chrono::system_clock::to_time_t(t0);
		std::string t(ctime(&t0_time));
		t = t.substr(0, t.length()-1);

		static std::mutex io_mutex;
		{
			std::lock_guard<std::mutex> lk(io_mutex);
			std::cout << '[' << t << "]: " << info << std::endl;
		}
	}

};



void my_thread(int id)
{
	Singleton &s = Singleton::get();
	for(uint32_t x=0; x<100; x++) {
		s.log("I'am thread " + std::to_string(id));
	}
}

int main()
{
	std::vector<std::thread> thread_list;
	for(uint32_t i=0; i<8; ++i) {
		thread_list.push_back(std::thread (my_thread, i));
	}

	Singleton::get().log("all threads spawned!");

	for(uint32_t i=0; i<8; ++i) {
		thread_list[i].join();
	}

	Singleton::get().log("all threads finished!");

	return 0;
}
