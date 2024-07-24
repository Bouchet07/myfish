#ifndef THREAD_H
#define THREAD_H

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

class ThreadPool {
public:
    void Start(const uint8_t num_threads);
    void QueueJob(const std::function<void()>& job);
    void Stop();
    bool busy();

private:
    void ThreadLoop();

    bool should_terminate = false;           // Tells threads to stop looking for jobs
    std::mutex queue_mutex;                  // Prevents data races to the job queue
    std::condition_variable mutex_condition; // Allows threads to wait on new jobs or termination 
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> jobs;
};

extern ThreadPool pool;
extern std::mutex mtx;

#endif // THREAD_H