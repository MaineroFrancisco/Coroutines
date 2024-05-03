# Resume

Follow through of [ACCU Conference talk](https://www.youtube.com/watch?v=Npiw4cYElng&ab_channel=ACCUConference) as a matter of practice for Coroutine handle and implementation using [C++ 20](https://en.cppreference.com/w/cpp/20) standard.

This repo is intended to expand that demo further from the basics as I continue to explore propeties of the Coroutines at my own rhythm.

---

# Key Concepts

- Awaited: Specify how async work is executed.
- Promise Type: Specifying how a coroutine operates.

## Awaiter type

Required functions to be defined for the awaiter structure.

- await_ready(): tells the system where suspend is ready.
- await_suspend():
-- Set up something to know to resume the coroutine.
-- Arrange for the work to eventually complete.
- await_resume(): produce the awaited result.
 
## Promise Type:

Minimum functions required:

- initial_suspend(): how to start a coroutine.
- final_suspend(): how to end a coroutine.
- unhandled_exception(): what to do with escaped exceptions.
- get_return_object(): how to build the result of factory function.
- return_void()/return_value() (missed on talk): to call a return function with void/non-void return type respectively.

---

For further references on Coroutines, take a look at corresponding [Cpp References](https://en.cppreference.com/w/cpp/language/coroutines) page.