#include "../profiler.h"
#include <iostream>
#include <thread>

DEFINE_API_PROFILER(__Short);
DEFINE_API_PROFILER(__Long);

// Arbitrary short function
int __Short()
{
    API_PROFILER(__Short);
    
    long long i = 0;
    
    while (i < 10) {
        int a = 1;
        i++;
    }
    
    return i;
}

// Arbitrary longer function
int __Long()
{
    API_PROFILER(__Long);
    
    long long i = 0;
    
    int a[25] = {0};
    int b[25] = {0};
    int c[25] = {0};
    
    while (i < 10) {
        a[i] = i;
        b[i] = i * i;
        
        double j = 0.0f;
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
int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <number_of_threads>\n";
        return 1;
    }
	
	int numThreads = std::atoi(argv[1]);

    if (numThreads <= 0)
    {
        std::cerr << "Invalid input. Please enter a positive integer.\n";
        return 1;
    }
	
	std::thread threads[numThreads];
	
    long long i = 0;
    
    for (int i = 0; i < numThreads; ++i)
    {
        threads[i] = std::thread([]() {
            long long i = 0;
            while (i < 5500000)
            {
                int x = __Short();
                int y = __Long();
                i++;
            }
        });
    }
	
	for (int i = 0; i < numThreads; ++i)
    {
        threads[i].join();
    }
    
    printf("Finished");

    return 0;
}
