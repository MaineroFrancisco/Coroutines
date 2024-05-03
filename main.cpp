#include <coroutine>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

// Template simple awaiter.
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

struct is_awaiter_test {
	struct promise_type {
		constexpr std::suspend_always initial_suspend() { return {}; };
		constexpr std::suspend_always final_suspend() noexcept { return {}; };
		void unhandled_exception() {}
		is_awaiter_test get_return_object() { return {}; }

		void return_void() {}
	};
};

// Use concept to distinguish between awaiting a value, and awaiting an awaiter.
template <typename T>
concept is_awaiter = requires { [](T t) -> is_awaiter_test { co_await t; }; };

struct task {
	struct promise_type {
		std::coroutine_handle<> continuation;

		std::exception_ptr error{};

		// Will cause main to print alone, because we never start the coroutine
		std::suspend_always initial_suspend() { return {}; };

		// std::suspend_never initial_suspend() { return {}; };

		// On final_suspend creates and awaiter which make stopped promise resume (to resume suspended task g())
		struct final_awaiter : std::suspend_always {
			promise_type *promise;
			void await_suspend(auto) noexcept { promise->continuation.resume(); }
		};
		final_awaiter final_suspend() noexcept { return {{}, this}; };

		task get_return_object() { return {this}; }
		void unhandled_exception() { error = std::current_exception(); }

		void return_void() {}

		// transform to await an awaiter.
		template <typename A>
			requires is_awaiter<A>
		auto await_transform(A a) {
			return a;
		}

		// transform to await a value.
		template <typename V>
			requires(!is_awaiter<V>)
		auto await_transform(V v) {
			return value_awaiter<V>(v);
		}
	};

	promise_type *promise;

	// Give a function to start the coroutine
	void start() {
		auto h = std::coroutine_handle<promise_type>::from_promise(*promise);
		h.resume();
	}

	struct nested_awaiter {
		promise_type *promise;
		bool await_ready() const {
			// std::cout << "nested_awaiter::await_ready() \n";
			return false;
		}
		void await_suspend(std::coroutine_handle<> continuation) const {
			promise->continuation = continuation;
			std::coroutine_handle<promise_type>::from_promise(*promise).resume();
		}
		void await_resume() {}
	};

	nested_awaiter operator co_await() { return nested_awaiter{promise}; }
};

int to_be_made_async() { return 17; }

task g(io &c) {
	std::cout << "second=" << co_await aync_read{c, 1} << "\n";
	std::cout << "third=" << co_await aync_read{c, 1} << "\n";
}

task f(io &c) {
	// co_await: Suspends a coroutine and returns control to the caller.
	std::cout << "first=" << co_await aync_read{c, 1} << "\n";
	std::cout << "value=" << co_await to_be_made_async() << "\n";
	co_await g(c);

	std::cout << "f end \n";
}

int main() {
	std::cout << std::unitbuf << std::boolalpha;

	try {
		io context;

		// Prints "first=", invoke async_read then awaits
		auto t = f(context);
		t.start();

		context.complete(1, "first line");
		std::cout << "Back in main \n";
		context.complete(1, "second line");
		context.complete(1, "third line");
	} catch (std::exception const &ex) {
		std::cout << "ERROR: " << ex.what() << "\n";
	}

	return 0;
}
