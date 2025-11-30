#pragma once
#include <vector>
#include <atomic>
#include <condition_variable>
#include <mutex>
#if defined(__GNUC__) || defined(__clang__)
// GCC/Clang specific
#include <x86intrin.h>   // or <arm_neon.h> depending on architecture
#elif defined(_MSC_VER)
// MSVC specific
#include <intrin.h>
#endif

#include <atomic>
#include <condition_variable>
#include <thread>
#include <functional>

namespace Hubris {
    class WaitGroup final {
    private:
        std::atomic<int> count{ 0 };            // Counter for tracking the number of tasks
        std::mutex mtx;                         // Mutex for coordinating condition variable
        std::condition_variable cv;             // Condition variable for wait signaling
        std::atomic<bool> waiting{ false };     // Flag to prevent multiple waits
    public:
        WaitGroup() = default;

        // Increment the counter
        void Add(int n) {
            count.fetch_add(n, std::memory_order_relaxed);
        }

        // Decrement the counter and notify waiting threads if it reaches zero
        void Done() {
            if (count.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                std::lock_guard<std::mutex> lock(mtx);
                cv.notify_all();  // Notify waiting threads
            }
        }

        // Wait for the counter to reach zero, usually called by the main thread
        void Wait() {
            bool expected = false;
            if (!waiting.compare_exchange_strong(expected, true)) {
                throw std::runtime_error("Only one thread can wait at a time.");
            }
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&] { return count.load(std::memory_order_acquire) == 0; });

            // Reset the waiting flag after the wait is done, out of precaution.
            waiting.store(false, std::memory_order_release);
        }
    };



    class ThreadPool final {
    private:
        /**
		* @brief The number of threads in the pool.
        */
		static inline unsigned int ThreadCount;
		/**
		* @brief The threads in the pool.
		*/
		static inline std::vector<std::thread> Threads;
    public:
        static void InitalizePool(unsigned int threadCount =  std::thread::hardware_concurrency() - 1) {
			ThreadCount = std::min(threadCount, std::thread::hardware_concurrency() - 1);
            Logger::Log("{} Threads Allocated", ThreadCount);
			for (unsigned int i = 0; i < ThreadCount; i++) {
				Threads.emplace_back([]() {
					while (true) {
						// Wait for a task to be assigned
						// Execute the task
					}
				});
			}
        }

        /**
         * @brief Queues a job to be executed by the threadpool, it may get executed by any thread in the pool.
         * 
         * (planned behaviour)
         * If wg is not null, the job is wrapped in a lambda that handles calling WaitGroup.Add() and WaitGroup.Done() appropriately.
         * 
         * @param wg This argument is here to support the upcoming (and completed) waitgroup  
         * @param job the job to be added to the Queue, it may execute on any thread in the pool.
         */
        static void QueueJob(WaitGroup* wg, std::function<void()> job){

        }
    };
}
