#include "Profiler.hpp"


#if ENABLE_API_PROFILER

#ifdef API_PROFILER_WINDOWS

APIProfiler::APIProfiler(ThreadInfo * threadInfo) {
  LARGE_INTEGER start;
  QueryPerformanceCounter( & start);
  m_start = start.QuadPart;
  m_threadInfo = threadInfo;
}

APIProfiler::~APIProfiler() {
  LARGE_INTEGER end;
  QueryPerformanceCounter(&end);
  m_threadInfo->accumulator += (end.QuadPart - m_start);
  m_threadInfo->hitCount++;
  if (end.QuadPart - m_threadInfo->lastReportTime > s_reportInterval)
    Flush(end.QuadPart);
}

// Initialize static class members
float APIProfiler::s_ooFrequency = 0;
INT64 APIProfiler::s_reportInterval = 0;

void APIProfiler::Flush(INT64 end) {
  if (s_reportInterval == 0) {
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    s_ooFrequency = 1.0f / freq.QuadPart;
    MemoryBarrier();
    s_reportInterval = static_cast < INT64 > (freq.QuadPart * 1.0f);
  }

  if (m_threadInfo->lastReportTime == 0) {
    m_threadInfo->lastReportTime = m_start;
    return;
  }

  float interval = (end - m_threadInfo->lastReportTime) * s_ooFrequency;
  float measured = m_threadInfo->accumulator * s_ooFrequency;

  printf("TID 0x%lx time spent in \"%s\": %.0f/%.0f microsec %.1f%% %lldx\n",
    GetCurrentThreadId(),
    m_threadInfo->name,
    measured * 1e6,
    interval * 1e6,
    100.0f * measured / interval,
    m_threadInfo->hitCount);

  m_threadInfo->lastReportTime = end;
  m_threadInfo->accumulator = 0;
  m_threadInfo->hitCount = 0;
}

#endif //API_PROFILER_WINDOWS

#ifdef API_PROFILER_UNIX

APIProfiler::APIProfiler(ThreadInfo * threadInfo) {
  clock_gettime(CLOCK_MONOTONIC, &m_start);
  m_threadInfo = threadInfo;
}

APIProfiler::~APIProfiler() {
  timespec end;
  clock_gettime(CLOCK_MONOTONIC, &end);

  long long startNanoSec = m_start.tv_sec * 1e9 + m_start.tv_nsec;
  long long endNanoSec = end.tv_sec * 1e9 + end.tv_nsec;
  long long lastReportNanoSec = m_threadInfo->lastReportTime.tv_sec * 1e9 + m_threadInfo->lastReportTime.tv_nsec;

  m_threadInfo->accumulator += (endNanoSec - startNanoSec) / 1000LL;
  m_threadInfo->hitCount++;

  if (((endNanoSec - lastReportNanoSec) / 1000LL) > s_reportInterval)
    Flush(end);
}

// Initialize static class members
long long APIProfiler::s_reportInterval = 0;

void APIProfiler::Flush(timespec end) {
  if (s_reportInterval == 0) {
    s_reportInterval = static_cast < long long > (1e6 * 1.0f);
  }

  if (m_threadInfo->lastReportTime.tv_sec == 0 && m_threadInfo->lastReportTime.tv_nsec == 0) {
    m_threadInfo->lastReportTime = m_start;
    return;
  }

  double interval = static_cast < double > ((end.tv_sec - m_threadInfo->lastReportTime.tv_sec) * 1e9 +
    (end.tv_nsec - m_threadInfo->lastReportTime.tv_nsec)) / 1000.0f;

  double measured = m_threadInfo->accumulator;

  printf("TID 0x%lx time spent in \"%s\": %.0f/%.0f microsec %.1f%% %lldx\n",
    (long) pthread_self(),
    m_threadInfo->name,
    measured,
    interval,
    100.0f * measured / interval,
    m_threadInfo->hitCount);

  m_threadInfo->lastReportTime = end;
  m_threadInfo->accumulator = 0;
  m_threadInfo->hitCount = 0;
}

#endif //API_PROFILER_UNIX
       
#endif //ENABLE_API_PROFILER
