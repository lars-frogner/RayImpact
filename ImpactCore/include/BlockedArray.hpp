#pragma once
#include "memory.hpp"

namespace Impact {

// BlockedArray declarations

template <typename T, unsigned int log2_block_extent>
class BlockedArray {

private:

    T* data; // Array of data, arranged in square blocks with extent 2^log2_block_extent
    unsigned int total_width; // Total number of values along the horizontal dimension
    unsigned int total_height; // Total number of values along the vertical dimension
    unsigned int n_blocks_horizontally; // Number of blocks horizontally

    constexpr unsigned int blockExtent() const;

    constexpr unsigned int blockSize() const;

    unsigned int roundedUp(unsigned int x) const;

    unsigned int get1DBlockIndex(unsigned int coordinate) const;

    unsigned int get1DOffsetInBlock(unsigned int coordinate) const;

    unsigned int getTotalOffset(unsigned int x, unsigned int y) const;

public:

    BlockedArray(unsigned int width, unsigned int height,
                 const T* initial_data = nullptr);

    ~BlockedArray();

    BlockedArray(const BlockedArray& other) = delete;

    BlockedArray& operator=(const BlockedArray& other) = delete;

    unsigned int width() const;

    unsigned int height() const;

    T& operator()(unsigned int x, unsigned int y);

    const T& operator()(unsigned int x, unsigned int y) const;
};

// BlockedArray inline method definitions

template <typename T, unsigned int log2_block_extent>
inline BlockedArray<T, log2_block_extent>::BlockedArray(unsigned int width, unsigned int height,
                                                        const T* initial_data /* = nullptr */)
    : total_width(width),
      total_height(height),
      n_blocks_horizontally(roundedUp(width) >> log2_block_extent)
{
    // The size of each dimension must be rounded up to contain a whole number of blocks
    size_t n_elements = roundedUp(width)*roundedUp(height);

    // Allocate memory for the blocked array
    data = allocateAligned<T>(n_elements);

    // Initialize all values
    for (size_t idx = 0; idx < size; idx++)
    {
        new (data + idx) T();
    }

    // Copy the contents of the initial data array into the blocked array
    if (initial_data)
    {
        for (unsigned int y = 0; y < height; y++) {
            for (unsigned int x = 0; x < width; x++)
            {
                (*this)(x, y) = initial_data[x + y*width];
            }
        }
    }
}

template <typename T, unsigned int log2_block_extent>
inline BlockedArray<T, log2_block_extent>::~BlockedArray()
{
    freeAligned(data);
}

// Computes the extent of a block
template <typename T, unsigned int log2_block_extent>
inline constexpr unsigned int BlockedArray<T, log2_block_extent>::blockExtent() const
{
    return 1 << log2_block_extent; // 2^log2_block_extent
}

// Computes the number of elements in a block
template <typename T, unsigned int log2_block_extent>
inline constexpr unsigned int BlockedArray<T, log2_block_extent>::blockSize() const
{
    return blockExtent()*blockExtent();
}

// Returns the given value rounded up to a multiple of the block extent
template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::roundedUp(unsigned int value) const
{
    return (value + blockSize() - 1) & ~(blockSize() - 1);
}

// Computes the horizontal/vertical index of the block containing the value with the given horizontal/vertical coordinate
template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::get1DBlockIndex(unsigned int coordinate) const
{
    return coordinate >> log2_block_extent; // idx/block_extent
}

// Computes the horizontal/vertical index of the given horizontal/vertical coordinate inside a block
template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::get1DOffsetInBlock(unsigned int coordinate) const
{
    return (coordinate & (blockSize() - 1));
}

template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::width() const
{
    return total_width;
}

template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::height() const
{
    return total_height;
}

template <typename T, unsigned int log2_block_extent>
inline unsigned int BlockedArray<T, log2_block_extent>::getTotalOffset(unsigned int x, unsigned int y) const
{
    unsigned int horizontal_block_idx = get1DBlockIndex(x);
    unsigned int vertical_block_idx = get1DBlockIndex(y);

    unsigned int horizontal_offset_in_block = get1DOffsetInBlock(x);
    unsigned int vertical_offset_in_block = get1DOffsetInBlock(y);

    // Compute offset for the beginning of the relevant block
    unsigned int offset = (horizontal_block_idx + vertical_block_idx*n_blocks_horizontally)*blockSize();

    // Add offset within block to obtain total offset
    offset += horizontal_offset_in_block + vertical_offset_in_block*blockExtent();

    return offset;
}

template <typename T, unsigned int log2_block_extent>
inline T& BlockedArray<T, log2_block_extent>::operator()(unsigned int x, unsigned int y)
{
    return data[offset(x, y)];
}

template <typename T, unsigned int log2_block_extent>
inline const T& BlockedArray<T, log2_block_extent>::operator()(unsigned int x, unsigned int y) const
{
    return data[offset(x, y)];
}

} // Impact
