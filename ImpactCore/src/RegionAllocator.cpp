#include "RegionAllocator.hpp"
#include "error.hpp"
#include "memory.hpp"
#include <algorithm>

namespace Impact {

RegionAllocator::RegionAllocator(size_t block_size /* = 262144 */)
	: block_size(block_size),
	  current_block(nullptr),
	  current_position(0),
	  current_block_size(0),
	  used_blocks(),
	  available_blocks()
{
	imp_assert(block_size % 16 == 0);
}

RegionAllocator::~RegionAllocator()
{
	auto iter = used_blocks.begin();

	for (; iter != used_blocks.end(); iter++)
	{
		freeAligned(iter->second);
		iter->second = nullptr;
	}

	for (iter = available_blocks.begin(); iter != available_blocks.end(); iter++)
	{
		freeAligned(iter->second);
		iter->second = nullptr;
	}

	if (current_block)
		freeAligned(current_block);
}

// Returns a pointer to a region of memory with room for the given number of bytes
void* RegionAllocator::allocate(size_t n_bytes)
{
	// Round up to multiple of 16 bytes to maintain alignment
	n_bytes = ((n_bytes + 15) & (~15)); // Adds 15 and then sets the last four bits to zero

	if (current_position + n_bytes > current_block_size)
	{
		// The current block does not have room for the requested allocation amount.

		// Add the current block to the list of full blocks
		if (current_block)
		{
			used_blocks.push_back(std::make_pair(current_block_size, current_block));
			current_block = nullptr;
		}

		// Check if there is an existing available block of sufficient size
		for (auto iter = available_blocks.begin(); iter != available_blocks.end(); iter++)
		{
			if (iter->first >= n_bytes)
			{
				// The available block is large enough for the requested allocation amount.

				// Make this the current block
				current_block_size = iter->first;
				current_block = iter->second;

				available_blocks.erase(iter);

				break;
			}
		}

		// Allocate a new block if no available blocks could be used
		if (!current_block)
		{
			// Make sure that the block is not too small to hold the requested allocation amount
			current_block_size = std::max(n_bytes, block_size);

			current_block = allocateAligned<uint8_t>(current_block_size);
		}

		// Current position is now the beginning of the new block
		current_position = 0;
	}

	// Create pointer to the allocated part of the block
	void* pointer_to_allocated = current_block + current_position;

	// Update current position
	current_position += n_bytes;

	return pointer_to_allocated;
}

// Returns a pointer to a region of memory with room for the given number of objects
template <typename T>
inline T* RegionAllocator::allocate(size_t n_elements /* = 1 */, bool call_constructor /* = true */)
{
	T* pointer_to_allocated = (T*)(allocate(n_elements*sizeof(T)));

	if (call_constructor)
	{
		for (size_t i = 0; i < n_elements; i++)
		{
			// Initialize the object of type T at memory position (pointer_to_allocated + i)
			new (pointer_to_allocated + i) T();
		}
	}
}

// Makes all of the currently allocated memory available for overwriting, invalidating any existing pointers
void RegionAllocator::release()
{
	// Reset position in current block
	current_position = 0;

	// Move all used blocks to the list of available blocks
	available_blocks.splice(available_blocks.begin(), used_blocks);
}

} // Impact
