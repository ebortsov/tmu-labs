# Sheet 06 - RAII, copy, move, rule of five

## `tempfs`

In this exercise, you are going to implement RAII wrappers for temporary files and directories through API calls.

On UNIX derivatives like Linux, _file descriptors_ are used as handles to access files and directories. A file
descriptor is a non-negative integer which is usually represented by the `int` type in C++. System calls that interact
with files may return file descriptors or take file descriptors as arguments as a way to identify specific files. For
example, the `open` POSIX API call opens or creates a file and returns a file descriptor which can be used for further
interactions with this file (such as reading from, writing to, or deleting the file).

In C++ terminology, such a file descriptor is a _resource_ with a defined lifetime &ndash; usually a file descriptor
stays valid until it is passed to the `close` API call. It is easy to
forget to invoke `close` on a file descriptor when it is no longer needed, which could result in resource leaks.
Therefore, file descriptors should be wrapped in suitable RAII types. Going one step further, temporary files and
directories themselves can be regarded as resources. When a temporary file or directory is no longer needed, its
lifetime ends and it should be deleted. This can also be achieved through suitable RAII wrappers.

In this exercise you should implement two RAII classes, `TempDirectory` and `TempFile`, with the following semantics:

- `TempDirectory` represents a temporary directory which can be created with an absolute path.
  On construction, `TempDirectory` creates the respective directory through suitable API calls (see below).
  On destruction, the directory is deleted again through API calls.

- `TempFile` represents a temporary file which can only be created within a
  `TempDirectory`. On construction, `TempFile` creates an empty file with a given name
  in the correct directory through suitable API calls. On destruction, the file is removed again
  through API calls.

You can assume that no other program can create files or directories
within the directories managed by the `TempDirectory` class, and that `TempDirectory` is never created
within another `TempDirectory`.

The `TempDirectory` must keep track of its children. Correct usage of your classes will make sure that the
`TempDirectory` is never destroyed before all of the `TempFile`s created within are also destroyed, but you must
guard against that incorrect usage. More specifically, if `TempDirectory` were to be deleted while there are files
inside, an assertion in the destructor should fail and kill the program using `<cassert>` and the `assert` macro.

Finally, the `TempDirectory` class should be neither copyable nor movable, whereas the
`TempFile` class should be movable but not copyable. Ensure that nobody else (except for your
classes themselves of course) can access the resources that are managed by your classes.

For your implementation you should use the API calls provided by the functions
that are declared in the header file `OSApi.hpp`. As described above, they use file
descriptors of type `int`. Negative values for the file descriptors
indicate an invalid descriptor or an error. If your implementation is correct,
the API functions should never generate errors. You can find more documentation
in the header file. The program scaffold for this exercise also includes the
other required header files.
You should always use the functions using parent directory file descriptors instead of full paths
when possible.

You will need to work with strings in your implementation in order to represent path- and filenames.
While it is possible to do this with C-style strings (i.e. `const char*`), this is
cumbersome and error-prone. You should instead use the C++ type `std::string` defined in
the `<string>` header. Among other operations `std::string` supports assignment from
C-style strings, copy- and move-semantics and concatenation through the binary `+` operator.
The `c_str()` member function can be used to access the underlying C-style string.

You are not allowed to modify the `osapi`. Fill out the skeletons in `TempFile.hpp` and `TempDirectory.hpp`,
and implement the classes in `TempFile.cpp` and `TempDirectory.cpp`.

## Scope

Logging and telemetry are indispensable in any complex system.
This week you will implement a structured approach to logging based on *scopes*.

The idea is to associate a particular segment of code (a function, a loop body, a critical block of logic)
with a scope that comes with a specialized logger and additional telemetry, like timing information.
RAII allows a handy implementation of this idiom &ndash; a scope is entered when it is created and exited when it goes
out of lexical scope.

Your implementation will allow logging messages associated with the scope name and ID, as well as automatically timing
segments of code by providing timestamps relative to start of scope on each message and at scope exit. Consider this
example:

```cpp
#include <format>
LogStream logs{};

void process() {
    Scope sc1{"process", logs};
    bool needsBlock2;
    {
        Scope sc2{"block1", logs};
        needsBlock2 = calculate();
        sc2.log(std::format("Result of calculate is {}", needsBlock2)); 
    }
    sc1.log("Continuing...");
    if (needsBlock2) {
        Scope sc3{"block2", logs};
        sc3.log("Starting computation...");
        int x = additionalComputation();
        sc3.log(std::format("Final result is {}", x));
    }
}
```

Assuming a simplified model where `calculate` takes 500ms and returns true, `additionalComputation()` takes 2s,
and each other instruction is instantaneous, we would expect to see the following logs:

```
[Info] Enter scope process (#1)
[Info] Enter scope block1 (#2)
[Info] block1 (#2) [+0.500s] Result of calculate is true
[Info] Exit block1 (#2), elapsed 0.500s
[Info] process (#1) [+0.500s] Continuing...
[Info] Enter scope block2 (#3)
[Info] block2 (#3) [+0.000s] Starting computation...
[Info] block2 (#3) [+2.000s] Final result is 42
[Info] Exit block2 (#3), elapsed 2.000s
[Info] Exit process (#1), elapsed 2.500s
```

You are given a header-only log library and skeletons for the definitions of
`Stopwatch`, `Duration`, and `Scope` classes. The only way to write to logs is through the `Logger::log` function
that is available only to the `Scope` class.

### `Duration`

The `Duration` class is a container for a time duration in nanoseconds. It should be trivially copyable and moveable
and provide basic utilities for addition and comparison, as well as accessors to the duration in various units.
Fill out the private details of `Duration` in `Stopwatch.hpp` and put the implementation in `Stopwatch.cpp`.

### `Stopwatch`

A stopwatch measures time since it was started and allows stopping the measurement of time at any point.
To find the current timestamp measured in nanoseconds use the `extern uint64_t get_clock_time()` function.
Do *not* define it, it will be provided and linked for you. You can assume the clock is monotonic, i.e. consecutive
calls to `get_clock_time` will return non-decreasing timestamps.

The `Stopwatch` has to be copy and move constructible and assignable.
Fill out the private details of `Stopwatch` in `Stopwatch.hpp` and put the implementation in `Stopwatch.cpp`.

### `Scope`

The `Scope` takes a reference to a `LogStream` and implements the scope idiom using RAII. As in the example,
it emits a special message on creation and exit, and measures time since the scope was entered &ndash; each log message
is accompanied by the time delta.

A `Scope` can be _anonymous_, in which case it does not have a name. However, each `Scope` has an ID that starts at 1
for the first created `Scope` and grows by one for each new `Scope`. This counter can be reset with the static
`resetIds` function.

The expected log messages for a named scope are:

- `[LEVEL] Enter scope NAME (#ID)` on entry,
- `[LEVEL] NAME (#ID) [+SECS.MILISs] MESSAGE` for each `log` call,
- `[LEVEL] Exit NAME (#ID), elapsed SECS.MILISs` on closure;

and for an anonymous scope:

- `[LEVEL] Enter anonymous scope (#ID)` on entry,
- `[LEVEL] (#ID) [+SECS.MILISs] MESSAGE` for each `log` call,
- `[LEVEL] Exit (#ID), elapsed SECS.MILISs` on closure;

where:

- `LEVEL` is a string representation of the log level ("Debug", "Info", "Warning", or "Error"),
- `NAME` is the name of the scope, as given in the constructor,
- `ID` is the ID of the scope,
- `SECS` is the number of fully elapsed seconds since scope start (see: `Duration::fullSeconds()`)
- `MILIS` is the _rounded_ number of milliseconds since scope start (see: `Duration::milliseconds()`),
- `MESSAGE` is the unchanged message string given to `log`.

Milliseconds must be formatted as 3-digit-wide, i.e. 1ms is `0.001ms`, 99ms is `0.099ms`, etc.

You can use `std::format` from `<format>` to nicely format the log messages. To output the number of milliseconds in
the required 3-digit-wide format you can use one of the format specifiers for integers &ndash; check the
[format specification](https://en.cppreference.com/w/cpp/utility/format/spec).

You can freely change the private implementation details in `Scope.hpp`. The implementation goes in `Scope.cpp`.