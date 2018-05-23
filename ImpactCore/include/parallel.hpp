#pragma once
#include "error.hpp"
#include <cstdint>
#include <functional>

namespace Impact {

// Global variables

extern unsigned int IMP_N_THREADS; // The number of threads used for parallelization
extern thread_local unsigned int IMP_THREAD_ID; // Unique thread identifier

// Parallel function declarations

void initializeParallel(unsigned int n_threads);

void cleanupParallel();

void parallelFor(const std::function<void (uint64_t)>& loop_body,
                 uint64_t n_iterations,
                 uint32_t chunk_size = 1);

void parallelFor2D(const std::function<void (uint32_t, uint32_t)>& loop_body,
                   uint32_t n_iterations_inner, uint32_t n_iterations_outer);

} // Impact
