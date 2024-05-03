#include <coroutine>
#include <exception>
#include <iostream>

template<typename V>
struct value_awaiter {
	V value;

	constexpr bool await_ready() { return true; }
	void await_suspend(auto) { }
	V await_resume() { return value; }
};

struct task {
	struct promise_type {
		std::exception_ptr error{};
		std::suspend_never initial_suspend() { return {}; };
		std::suspend_never final_suspend() noexcept { return {}; };
		task get_return_object() { return {}; }
		void unhandled_exception() { error = std::current_exception(); }

		void return_void() {}

		template<typename V>
		auto await_transform(V v) { return value_awaiter<V>(v); }
	};
};

task f() {
	std::cout << "value=" << co_await 17 << "\n";
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
