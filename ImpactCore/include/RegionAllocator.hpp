#pragma once
#include <cstdint>
#include <list>
#include <utility>

namespace Impact {

// RegionAllocator declarations

class RegionAllocator {

private:

    const size_t block_size; // Size of each memory block (default is 256 kB)
    uint8_t* current_block; // Latest allocated memory block
    size_t current_position; // Offset within the current block to next free memory position
    size_t current_block_size; // Size of the current block (usually equal to block_size)
    std::list< std::pair<size_t, uint8_t*> > used_blocks; // List of used blocks and their sizes
    std::list< std::pair<size_t, uint8_t*> > available_blocks; // List of allocated but unused blocks and their sizes
    
public:

    RegionAllocator(size_t block_size = 262144);

    ~RegionAllocator();

    void* allocate(size_t n_bytes);

    template <typename T>
    T* allocate(size_t n_elements = 1, bool call_constructor = true);

    void release();
};

} // Impact
