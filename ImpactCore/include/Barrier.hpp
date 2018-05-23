#pragma once
#include "error.hpp"
#include <mutex>

namespace Impact {

// Barrier declarations

class Barrier {

private:
	const unsigned int total_n_threads;
	unsigned int remaining_n_threads;
	std::mutex mutex;
	std::condition_variable condition;

public:

	Barrier(unsigned int total_n_threads);
	~Barrier();

	void reset();
	
	void wait();
};

// Barrier inline method definitions

inline Barrier::Barrier(unsigned int total_n_threads)
	: total_n_threads(total_n_threads),
	  remaining_n_threads(total_n_threads)
{
	imp_assert(total_n_threads > 0);
}

inline Barrier::~Barrier()
{
	imp_assert(remaining_n_threads == 0);
}

inline void Barrier::reset()
{
	imp_assert(remaining_n_threads == 0);
	remaining_n_threads = total_n_threads;
}

} // Impact