# Resume

Follow through of [ACCU Conference talk](https://www.youtube.com/watch?v=Npiw4cYElng&ab_channel=ACCUConference) as a matter of practice for Coroutine handle and implementation using [C++ 20](https://en.cppreference.com/w/cpp/20) standard.

This repo is intended to expand that demo further from the basics as I continue to explore propeties of the Coroutines at my own rhythm.

---

# Key Concepts

<ul>
<li>Awaited: Specify how async work is executed.</li>
<li>Promise Type: Specifying how a coroutine operates.</li>
</ul>

## Awaiter type

Required functions to be defined for the awaiter structure.

<ul>
 <li>await_ready(): tells the system where suspend is ready.</li>
 <li>await_suspend():</li>
 <ul>
  <li>Set up something to know to resume the coroutine.</li>
  <li>Arrange for the work to eventually complete.</li>
 </ul>
 <li>await_resume(): produce the awaited result.</li>
</ul>

## Promise Type:

Minimum functions required:

<ul>
<li>initial_suspend(): how to start a coroutine.</li>
<li>final_suspend(): how to end a coroutine.</li>
<li>unhandled_exception(): what to do with escaped exceptions.</li>
<li>get_return_object(): how to build the result of factory function.</li>
<li>return_void()/return_value() (missed on talk): to call a return function with void/non-void return type respectively.</li>
</ul>

---

For further references on Coroutines, take a look at corresponding [Cpp References](https://en.cppreference.com/w/cpp/language/coroutines) page.
