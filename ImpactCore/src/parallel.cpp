#include "parallel.hpp"
#include "error.hpp"
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

namespace Impact {

// Forward declarations
class ParallelForLoop;

// Static global variables

static std::vector<std::thread> threads; // Pool of persistent threads
unsigned int IMP_N_THREADS; // The number of threads used for parallelization
thread_local unsigned int IMP_THREAD_ID; // Unique thread identifier

static bool terminate_threads = false; // Whether the persistent threads should be terminated

static ParallelForLoop* pending_loops = nullptr; // Pointer to first element in a list of unfinished loops
static std::mutex pending_loops_mutex; // Mutex for the loops list that should always be owned when modifying the list
static std::condition_variable pending_loops_condition;

// ParallelForLoop declarations

class ParallelForLoop {

public:

    std::function<void (uint64_t)> loop_body_1D; // Loop body function for 1D loops
    std::function<void (uint32_t, uint32_t)> loop_body_2D; // Loop body function for 2D loops
    uint64_t max_loop_index; // One more than the largest loop index to execute
    uint64_t max_inner_loop_index; // One more than the largest inner loop index to execute
    const unsigned int chunk_size; // Minimum number of contiguous loop iterations to perform at a time

    uint64_t next_loop_index; // Next loop index to execute
    unsigned int number_of_active_workers; // Number of threads currently executing iterations of the loop
    ParallelForLoop* next_loop; // Pointer to next loop in the list of unfinished loops

    ParallelForLoop(const std::function<void (uint64_t)>& loop_body_1D,
                    uint64_t n_iterations,
                    unsigned int chunk_size);

    ParallelForLoop(const std::function<void (uint32_t, uint32_t)>& loop_body_2D,
                    uint32_t n_iterations_inner, uint32_t n_iterations_outer);

    bool isFinished() const;
};

// ParallelForLoop method implementations

ParallelForLoop::ParallelForLoop(const std::function<void (uint64_t)>& loop_body_1D,
                                 uint64_t n_iterations,
                                 unsigned int chunk_size)
    : loop_body_1D(loop_body_1D),
      max_loop_index(n_iterations),
      max_inner_loop_index(n_iterations),
      chunk_size(chunk_size),
      next_loop_index(0),
      number_of_active_workers(0),
      next_loop(nullptr)
{}

ParallelForLoop::ParallelForLoop(const std::function<void (uint32_t, uint32_t)>& loop_body_2D,
                                 uint32_t n_iterations_inner, uint32_t n_iterations_outer)
    : loop_body_2D(loop_body_2D),
      max_loop_index(n_iterations_inner*n_iterations_outer),
      max_inner_loop_index(n_iterations_inner),
      chunk_size(1),
      next_loop_index(0),
      number_of_active_workers(0),
      next_loop(nullptr)
{}

// Whether all iterations of the loop have been completed
bool ParallelForLoop::isFinished() const
{
    return next_loop_index >= max_loop_index && number_of_active_workers == 0;
}

// Functions for parallelism

static void threadExecutionFunction(unsigned int id)
{
    // Update globally visible thread id
    IMP_THREAD_ID = id;

    // Initialize a lock object associated with the loops mutex
    std::unique_lock<std::mutex> lock(pending_loops_mutex);

    while (!terminate_threads) // Repeat until the threads are supposed to terminate
    {
        if (!pending_loops) // If the list of unfinished loops is empth
        {
            // Wait until notified that there is available work.
            pending_loops_condition.wait(lock);

            // Before returning from the call, this thread is given ownership of the loops mutex lock.
        }
        else
        {
            ParallelForLoop& loop = *pending_loops;

            // Compute range of indices for the iterations to perform with this thread
            uint64_t start_index = loop.next_loop_index;
            uint64_t end_index = std::min(start_index + loop.chunk_size, loop.max_loop_index);

            // Set next loop index to be executed in the loop
            loop.next_loop_index = end_index;

            // If the end of the loop will be reached when this thread is done,
            // move the next unfinished loop to the front of the list
            if (loop.next_loop_index == loop.max_loop_index)
                pending_loops = loop.next_loop;

            // Inform that this thread is currently working on the loop
            loop.number_of_active_workers++;

            // Release ownership of the lock on the loops mutex, enabling other
            // threads to take ownership so they can get on with their work
            lock.unlock();

            // Perform loop iterations
            for (uint64_t i = start_index; i < end_index; i++)
            {
                if (loop.loop_body_1D)
                {
                    loop.loop_body_1D(i);
                }
                else
                {
                    imp_check(loop.loop_body_2D);
                    loop.loop_body_2D((uint32_t)(i % loop.max_inner_loop_index), (uint32_t)(i/loop.max_inner_loop_index));
                }
            }

            // Wait here until ownership of the mutex lock can be reaquired
            lock.lock();

            // Inform that this thread is not working on the loop anymore
            loop.number_of_active_workers--;

            // If the current loop is finished, notify the waiting workers
            if (loop.isFinished())
                pending_loops_condition.notify_all();
        }
    }
}

// Forks threads for parallel execution and performs required initializations
void initializeParallel(unsigned int n_threads)
{
    imp_check(threads.empty());

    // Find total number of threads to use
    IMP_N_THREADS = (n_threads == 0)? std::max(1u, std::thread::hardware_concurrency()) : n_threads;

    IMP_THREAD_ID = 0; // Give main thread an id of 0

    // Create worker threads
    for (unsigned int id = 1; id < IMP_N_THREADS; id++)
    {
        threads.push_back(std::thread(threadExecutionFunction, id));
    }
}

// Joins threads and performs required clean up
void cleanupParallel()
{
    if (threads.empty())
        return;

    // Take ownership of the loops mutex lock (subsequent threads trying
    // to take ownership will hold until ownership is released)
    pending_loops_mutex.lock();

    // Make worker threads return from threadExecutionFunction
    terminate_threads = true;
    pending_loops_condition.notify_all();

    // Release ownership of the loops mutex lock
    pending_loops_mutex.unlock();

    // Join threads
    for (std::thread& thread : threads)
        thread.join();

    // Clear list of threads
    threads.erase(threads.begin(), threads.end());

    // Reset to initial state
    terminate_threads = false;

    IMP_N_THREADS = 1;
}

// Executes the given loop body function (taking the loop index as agument) in parallel
// for the given number of iterations
void parallelFor(const std::function<void (uint64_t)>& loop_body,
                 uint64_t n_iterations,
                 unsigned int chunk_size /* = 1 */)
{
    imp_check(!threads.empty() || IMP_N_THREADS == 1);

    // Perform iterations in serial if there are just a few of them (or only one thread)
    if (threads.empty() || n_iterations < chunk_size)
    {
        for (unsigned int i = 0; i < n_iterations; i++)
            loop_body(i);

        return;
    }

    // Create parallel for loop object
    ParallelForLoop loop(loop_body, n_iterations, chunk_size);

    // Take ownership of the loops mutex lock (subsequent threads trying
    // to take ownership will hold until ownership is released)
    pending_loops_mutex.lock();

    // Append the loop to the beginning of the list of unfinished loops
    loop.next_loop = pending_loops;
    pending_loops = &loop;

    // Release ownership of the loops mutex lock
    pending_loops_mutex.unlock();

    // Initialize a lock object associated with the loops mutex and take ownership of the lock
    std::unique_lock<std::mutex> lock(pending_loops_mutex);

    // Notify all the worker threads that there is available work
    pending_loops_condition.notify_all();

    while (!loop.isFinished())
    {
        // Compute range of indices for the iterations to perform with this thread
        uint64_t start_index = loop.next_loop_index;
        uint64_t end_index = std::min(start_index + loop.chunk_size, loop.max_loop_index);

        // Set next loop index to be executed in the loop
        loop.next_loop_index = end_index;

        // If the end of the loop will be reached when this thread is done,
        // move the next unfinished loop to the front of the list
        if (loop.next_loop_index == loop.max_loop_index)
            pending_loops = loop.next_loop;

        // Inform that this thread is currently working on the loop
        loop.number_of_active_workers++;

        // Release ownership of the lock on the loops mutex, enabling other
        // threads to take ownership so they can get on with their work
        lock.unlock();

        // Perform loop iterations
        for (uint64_t i = start_index; i < end_index; i++)
        {
            if (loop.loop_body_1D)
            {
                loop.loop_body_1D(i);
            }
            else
            {
                imp_check(loop.loop_body_2D);
                loop.loop_body_2D((uint32_t)(i % loop.max_inner_loop_index), (uint32_t)(i/loop.max_inner_loop_index));
            }
        }

        // Wait here until ownership of the mutex lock can be reaquired
        lock.lock();

        // Inform that this thread is not working on the loop anymore
        loop.number_of_active_workers--;
    }
}

// Executes the given 2D loop body function (taking the loop indices as aguments) in parallel
// for the given number of inner and outer iterations
void parallelFor2D(const std::function<void (uint32_t, uint32_t)>& loop_body,
                   uint32_t n_iterations_inner, uint32_t n_iterations_outer)
{
    imp_check(!threads.empty() || IMP_N_THREADS == 1);

    // Perform iterations in serial if there is just one of them (or only one thread)
    if (threads.empty() || n_iterations_inner*n_iterations_outer <= 1)
    {
        for (uint32_t j = 0; j < n_iterations_outer; j++)
            for (uint32_t i = 0; i < n_iterations_inner; i++)
                loop_body(i, j);

        return;
    }

    // Create parallel for loop object
    ParallelForLoop loop(loop_body, n_iterations_inner, n_iterations_outer);

    // Take ownership of the loops mutex lock (subsequent threads trying
    // to take ownership will hold until ownership is released)
    pending_loops_mutex.lock();

    // Append the loop to the beginning of the list of unfinished loops
    loop.next_loop = pending_loops;
    pending_loops = &loop;

    // Release ownership of the loops mutex lock
    pending_loops_mutex.unlock();

    // Initialize a lock object associated with the loops mutex and take ownership of the lock
    std::unique_lock<std::mutex> lock(pending_loops_mutex);

    // Notify all the worker threads that there is available work
    pending_loops_condition.notify_all();

    while (!loop.isFinished())
    {
        // Compute range of indices for the iterations to perform with this thread
        uint64_t start_index = loop.next_loop_index;
        uint64_t end_index = std::min(start_index + loop.chunk_size, loop.max_loop_index);

        // Set next loop index to be executed in the loop
        loop.next_loop_index = end_index;

        // If the end of the loop will be reached when this thread is done,
        // move the next unfinished loop to the front of the list
        if (loop.next_loop_index == loop.max_loop_index)
            pending_loops = loop.next_loop;

        // Inform that this thread is currently working on the loop
        loop.number_of_active_workers++;

        // Release ownership of the lock on the loops mutex, enabling other
        // threads to take ownership so they can get on with their work
        lock.unlock();

        // Perform loop iterations
        for (uint64_t i = start_index; i < end_index; i++)
        {
            if (loop.loop_body_1D)
            {
                loop.loop_body_1D(i);
            }
            else
            {
                imp_check(loop.loop_body_2D);
                loop.loop_body_2D((uint32_t)(i % loop.max_inner_loop_index), (uint32_t)(i/loop.max_inner_loop_index));
            }
        }

        // Wait here until ownership of the mutex lock can be reaquired
        lock.lock();

        // Inform that this thread is not working on the loop anymore
        loop.number_of_active_workers--;
    }
}

} // Impact
