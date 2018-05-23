#include "AtomicFloat.hpp"

namespace Impact {

// AtomicFloat method definitions

void AtomicFloat::add(imp_float value)
{
    imp_float_bits old_bits = bits;
    imp_float_bits new_bits;

    do {

        // Compute the sum
        new_bits = floatToBits(bitsToFloat(old_bits) + value);

      // Try assigning new_bits to bits. If bits has been changed
      // by another thread and thus differs from old_bits,
      // old_bits is updated with the current value of bits,
      // the sum is recomputed and another attempt is made.
    } while (!bits.compare_exchange_weak(old_bits, new_bits));
}

} // Impact
