#pragma once
#include <cstdint>
#include <functional>

namespace Impact {

extern unsigned int IMP_N_THREADS; // The number of threads used for parallelization
	
extern thread_local unsigned int IMP_THREAD_ID; // Unique thread identifier

void initializeParallel(unsigned int n_threads);

void cleanupParallel();

void parallelFor(const std::function<void (uint64_t)>& loop_body,
				 uint64_t n_iterations,
				 unsigned int chunk_size = 1);

void parallelFor2D(const std::function<void (uint64_t, uint64_t)>& loop_body,
				   uint64_t n_iterations_inner, uint64_t n_iterations_outer);

} // Impact
