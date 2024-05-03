#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

// Template simple awaiter
template <typename V> struct value_awaiter {
	V value;

	constexpr bool await_ready() const { return true; }
	void await_suspend(auto) {}
	V await_resume() { return value; }
};

struct io {
	std::unordered_map<int, std::function<void(std::string)>> outstanding;

	void submit(int fd, auto func) { outstanding[fd] = func; }
	void complete(int fd, std::string value) {
		auto it = outstanding.find(fd);
		if (it != outstanding.end()) {
			// Catches the function to be called after erase, so func() doesn't resume at this point and erase
			// inmediatly after (not prompting the second line output)
			auto fun = it->second;

			outstanding.erase(it);
			fun(value);
		}
	}
};

// Simulate an async IO file read procedure
struct aync_read {
	io &context;
	int fd; // File descriptor

	std::string value;

	constexpr bool await_ready() const { return false; }
	void await_suspend(std::coroutine_handle<> h) {
		context.submit(fd, [this, h](std::string line) {
			value = line;
			h.resume();
		});
	}
	constexpr std::string await_resume() { return value; }
};

struct task {
	struct promise_type {
		std::exception_ptr error{};
		std::suspend_never initial_suspend() { return {}; };
		std::suspend_never final_suspend() noexcept { return {}; };
		task get_return_object() { return {}; }
		void unhandled_exception() { error = std::current_exception(); }

		void return_void() {}

		// template <typename V> auto await_transform(V v) { return value_awaiter<V>(v); }
	};
};

task f(io &c) {
	// co_await: Suspends a coroutine and returns control to the caller.
	std::string message = co_await aync_read{c, 1};
	std::cout << "first=" << message << "\n";
	message = co_await aync_read{c, 1};
	std::cout << "second=" << message << "\n";
}

int main() {
	std::cout << std::unitbuf << std::boolalpha;

	try {
		io context;

		// Prints "first=", invoke async_read then awaits
		f(context);

		// Resume execution, finish printing "first=first line", then f() proceeds and awaits on aync_read again
		context.complete(1, "first line");
		// Main continues printing "Back to main"
		std::cout << "Back in main \n";
		// Resume execution and prints "second=second line"
		context.complete(1, "second line");
	} catch (std::exception const &ex) {
		std::cout << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
