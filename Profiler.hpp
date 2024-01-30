#ifndef PROFILER_H
#define PROFILER_H

// Comment this out or set to zero to disable
#define ENABLE_API_PROFILER 1

#if ENABLE_API_PROFILER

#if defined(_WIN32) || defined(_WIN64)

#define API_PROFILER_WINDOWS
#include <profileapi.h>         // QueryPerformanceCounter and QueryPerformanceFrequency
#include <winnt.h>              // MemoryBarrier and LARGE_INTEGER
#include <processthreadsapi.h>  // GetCurrentThreadId

#else

#define API_PROFILER_UNIX
#include <time.h>

#endif

#include <cstdio>
#include <pthread.h>

#ifdef API_PROFILER_WINDOWS

class APIProfiler {

public:

  struct ThreadInfo {
    INT64 lastReportTime;
    INT64 accumulator;
    INT64 hitCount;
    const char * name;
  };
  
  APIProfiler(ThreadInfo * threadInfo);
  ~APIProfiler();
  
private:

  ThreadInfo * m_threadInfo;
  INT64 m_start;
  static float s_ooFrequency;
  static INT64 s_reportInterval;
  
  void Flush(INT64 end);
};


#endif //API_PROFILER_WINDOWS

#ifdef API_PROFILER_UNIX

class APIProfiler {

public:

  struct ThreadInfo {
    timespec lastReportTime;
    long long accumulator;
    long long hitCount;
    const char * name;
  };

  APIProfiler(ThreadInfo * threadInfo);
  ~APIProfiler();

private:

  ThreadInfo * m_threadInfo;
  timespec m_start;
  static long long s_reportInterval;
  
  void Flush(timespec end);
};


#endif //API_PROFILER_UNIX

#ifdef API_PROFILER_WINDOWS

#define DECLARE_API_PROFILER(name)\
	extern thread_local APIProfiler::ThreadInfo __APIProfiler_##name;

#define DEFINE_API_PROFILER(name)\
	thread_local APIProfiler::ThreadInfo __APIProfiler_##name = { 0, 0, 0, #name };
	
#endif //API_PROFILER_WINDOWS

#ifdef API_PROFILER_UNIX

#define DECLARE_API_PROFILER(name)\
	extern __thread APIProfiler::ThreadInfo __APIProfiler_##name;

#define DEFINE_API_PROFILER(name)\
	__thread APIProfiler::ThreadInfo __APIProfiler_##name = { { 0, 0 }, 0, 0, #name };
	
#endif //API_PROFILER_UNIX

#define TOKENPASTE2(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE2(x, y)

#define API_PROFILER(name)\
    APIProfiler TOKENPASTE(__APIProfiler_##name, __LINE__)(&__APIProfiler_##name)

#else // Macros evaluate to nothing when profiler disabled

#define DECLARE_API_PROFILER(name)
#define DEFINE_API_PROFILER(name)
#define API_PROFILER(name)

#endif // ENABLE_API_PROFILER

#endif //PROFILER_H
