#pragma once
#include "precision.hpp"
#include <atomic>

namespace Impact {

// AtomicFloat declarations

class AtomicFloat {

private:

	std::atomic<imp_float_bits> bits;

public:

	explicit AtomicFloat(imp_float value = 0.0f);

	operator imp_float() const;

	imp_float operator=(imp_float value);

	void add(imp_float value);
};

} // Impact
