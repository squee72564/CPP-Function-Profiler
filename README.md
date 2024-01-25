
# CPP-Multithreaded-Profiler

This is a header only C++ tool to help profile multi-threaded applications. It is compatible with Windows OS and is being extended to work on Mac/Linux.

## Installation
1. `git clone https://github.com/squee72564/CPP-Multithreading-Profiler.git` to download this repository
2. Change current directory to the directory for the newly cloned repo
3. Move/copy the `profiler.h` file to the appropriate place within your personal C++ project
4. When compiling be sure to include this header file if any profiler macros are used within the project

## Using the Profiler
There are some examples within the `tests` folder, but the general approach to using the profiler is as follows:

This particular profiling module is meant to act on one or more _target modules_ in the application. A target module can be anything which exposes a well-defined API. To make it work, you must insert a macro named `API_PROFILER` into every public function exposed by that API:
```
DEFINE_API_PROFILER(foo);

void foo()
{
    API_PROFILER(foo);
            ...
}
```
The macro takes a single argument, which is just an identifier for the target module being profiled. For this to be a valid identifier, you must place exactly one  `DEFINE_API_PROFILER`  macro at global scope, as seen above. You can also insert  `DECLARE_API_PROFILER`  anywhere at global scope, perhaps in a header file, in the same way that you’d forward declare a global variable or function.

### Important!
The `API_PROFILER` macro creates a C++ object on the stack and it is important not to call this macro recursively. In other words, don’t insert `API_PROFILER` anywhere that might be called within the scope of another `API_PROFILER` marker, using the same identifier. If you do, you’ll end up counting the time spent inside the target module twice.

When the application runs, each thread will automatically log performance statistics once per second, including the thread identifier (TID), time spent inside the target module, and the number of calls. Here, we see performance statistics across six different threads:

```
TID 0x13bc time spent in "foo": 41148/1000000 microsec 4.1% 1280447x
TID 0x1244 time spent in "foo": 41240/1008886 microsec 4.1% 1280057x
TID 0x198 time spent in "foo": 41240/1008886 microsec 4.1% 1280057x 
TID 0x11d0 time spent in "foo": 40768/1000000 microsec 4.1% 1260806x 
TID 0x12a4 time spent in "foo": 40822/1000000 microsec 4.1% 1273203x 
TID 0xc14 time spent in "foo":  40715/1000000 microsec 4.1% 1266852x
```

Credit to Jeff Preshing at https://preshing.com/ for the original profiler idea and code, I am just extending it for other operating systems.