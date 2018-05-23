#include "Barrier.hpp"

namespace Impact {

// Barrier method definitions
	
void Barrier::wait()
{
    imp_assert(remaining_n_threads > 0);

	// Aquire a lock on the mutex
    std::unique_lock<std::mutex> lock(mutex);

    if (--remaining_n_threads == 0)
        // This is the last thread to reach the barrier; wake up all of the
        // other ones before exiting.
        condition.notify_all();
    else
        // Otherwise there are still threads that haven't reached it. Give
        // up the lock and wait to be notified.
        condition.wait(lock, [this] { return remaining_n_threads == 0; });
}

} // Impact