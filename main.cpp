#include <coroutine>
#include <exception>
#include <iostream>

struct task {
	struct promise_type {
		std::exception_ptr error{};
		std::suspend_never initial_suspend() { return {}; };
		std::suspend_never final_suspend() noexcept { return {}; };
		task get_return_object() { return {}; }
		void unhandled_exception() { error = std::current_exception(); }
		//void return_void() {}
	};
};

struct value_awaiter {
	int value;

	constexpr bool await_ready() { return true; }
	void await_suspend (auto) { }
	int await_resume() { return value; }
};

task f() {
	std::cout << "value=" << co_await value_awaiter(17) << "\n";
}

int main() {
	std::cout << std::unitbuf << std::boolalpha;

	try {
		f();
		std::cout << "Hello World! \n";
	}
	catch (std::exception const& ex) {
		std::cout << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
