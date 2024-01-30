#include "../Profiler.hpp"
#include <iostream>
#include <thread>
#include <atomic>

DEFINE_API_PROFILER(__Short);
DEFINE_API_PROFILER(__Long);

// Arbitrary short function
int __Short() {
  API_PROFILER(__Short);

  long long i = 0;

  int a = i;

  while (i < 10) {
    a += i;
    i++;
  }

  return a;
}

// Arbitrary longer function
int __Long() {
  API_PROFILER(__Long);

  long long i = 0;

  int a[25] = {0};
  int b[25] = {0};
  int c[25] = {0};

  while (i < 10) {
    a[i] = i;
    b[i] = i * i;

    for (int k = 0; k < 20; k++) {
      if (b[i] != 0) {
        c[i] += a[i] / b[i];
      } else {
        c[i] += a[i];
      }
    }
    i++;
  }

  return c[24];
}

// Main function
int main(int argc, char * argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <number_of_threads>\n";
    return 1;
  }

  int numThreads = std::atoi(argv[1]);

  if (numThreads <= 0) {
    std::cerr << "Invalid input. Please enter a positive integer.\n";
    return 1;
  }

  std::thread threads[numThreads];

  std::atomic < long > total(0);

  for (int i = 0; i < numThreads; ++i) {
    threads[i] = std::thread([&total]() {
      long long j = 0;
      while (j < 5500000) {
        int x = __Short();
        int y = __Long();
        total += x + y;
        j++;
      }
    });
  }

  for (int i = 0; i < numThreads; ++i) {
    threads[i].join();
  }

  printf("Finished with total: %ld\n", total.load());

  return 0;
}
