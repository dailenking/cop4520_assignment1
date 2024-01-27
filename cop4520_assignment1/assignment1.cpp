
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <cmath>
#include <mutex>
#include <memory>
#include <future>
#include <chrono>

using namespace std;

/*

APPROACH

Once the file is executed a timer immediately starts.

Seeing as 10^8 is a constant number, it best to define it in a constant variable able to 
be accessed and unchanged by the program.

The next step was to define a vector of threads and a mutex object so the threads wont
simultaneously access and change the primes_thread vector. 

Following this, I split 10^8 into even chunks so each thread has an equal amount of numbers
to perform the necessary calculations while also completing the calculation within the same
time period.

Each thread is then executed, given a start and end range to perform the prime calculation on.

Once all threads are done, the timer stops and the necessary information is written to a primes.txt
file.

***********************

EFFICIENCY

This program efficiently handles and manages threads in a vector and handles the simultaneous access
and changing of the primes_threads vector through a mutex object. 

Each thread is also given an equal amount of numbers to perform calculations on through the "divide_among_threads"
variable.

*/


// global variables
long long int sum = 0; // sum of all primes
long long int num_of_primes = 0; // number of primes

class Multithreading {



    public:
       static void spawnThreads(int num_threads) 
        {
            const int number_to_consider = 100000000; // 10^8 as int

            // vector of threads
            vector<unique_ptr<thread> > primes_threads;
            
            // mutex object (so objects can't access prime_threads vector simultaneously)
            static mutex mtx; 

            const int divide_among_threads = number_to_consider / num_threads;

            // Spawn threads using std::async
            for (int i = 0; i < num_threads; ++i) 
            {
                int start = i * divide_among_threads + 1;
                int end = (i + 1) * divide_among_threads;

                // creating thread
                primes_threads.emplace_back(make_unique<thread>(&Multithreading::threadFunction, start, end, std::ref(mtx)));
            }

            // Wait for all tasks to finish
            for (auto& t : primes_threads) {
                t->join();
            }

        }

        // Static member function for thread logic
        static void threadFunction(int start, int end, mutex& mtx) 
        {

        // Perform prime number calculation for the given range
        vector<int> primes;
        for (int num = start; num <= end; ++num) {
            if (primesCalculation(num)) {
                lock_guard<mutex> lock(mtx);
                primes.push_back(num);
            }
        }
        }
    
    public:
        static bool primesCalculation(int num)
        {
            if (num <= 1) 
                return false;
        

            for (int i = 2; i <= std::sqrt(num); ++i) 
            {
                if (num % i == 0) 
                {
                    return false;
                }
            }

            sum += num; // updating sum
            num_of_primes++; // updating number of primes

            return true;

        }

    public:    
        static void printToFile(const std::chrono::microseconds& duration)
        {
            ofstream PrimeFile("primes.txt");
            
            PrimeFile << duration.count() << " " << num_of_primes << " " << sum;


            PrimeFile.close();
        }

};

int main(int argc, char *argv[]) 
{
    // starting program clock
    auto start = std::chrono::high_resolution_clock::now();

    Multithreading multithreading;

    // spawn 8 threads
    multithreading.spawnThreads(8);

    auto stop = std::chrono::high_resolution_clock::now();

    // Calculate the duration in microseconds
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);


    // print information to primes.txt
    multithreading.printToFile(duration);


    return 0;
}