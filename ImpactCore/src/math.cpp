#include "math.hpp"
#include "error.hpp"
#include <utility>
#include <cmath>

namespace Impact {
	
// Math function definitions

// Solves the quadratic equation a*x^2 + b*x + c = 0 for the solutions x1 (smallest) and x2 (largest) if they exist
bool solveQuadraticEquation(imp_float a, imp_float b, imp_float c, imp_float* x1, imp_float* x2)
{
    imp_assert(x1 && x2);
    imp_assert(a != 0);

    double discriminant = (double)(b)*(double)(b) - 4.0*(double)(a)*(double)(c);

    if (discriminant < 0)
        return false;

    double sqrt_discriminant = std::sqrt(discriminant);

    double q = (b < 0)? -0.5*(b - sqrt_discriminant) : -0.5*(b + sqrt_discriminant);

    *x1 = (imp_float)(q/a);
    *x2 = (imp_float)(c/q);

    if (*x1 > *x2) std::swap(*x1, *x2);

    return true;
}

// Solves the the linear system A[i][0]*x0 + A[i][1]*x1 = b[i], i = 0, 1 and returns false if the solution doesn't exist
bool solve2x2LinearSystem(imp_float A[2][2], imp_float b[2], imp_float* x0, imp_float* x1)
{
    imp_float inverse_determinant = A[0][0]*A[1][1] - A[0][1]*A[1][0];

    if (std::abs(inverse_determinant) < 1e-10f)
        return false;

    inverse_determinant = 1.0f/inverse_determinant;

    *x0 = (A[1][1]*b[0] - A[0][1]*b[1])*inverse_determinant;
    *x1 = (A[0][0]*b[1] - A[1][0]*b[0])*inverse_determinant;

    if (std::isnan(*x0) || std::isnan(*x1))
        return false;

    return true;
}

// Returns the last index in an array of size n_values where the given condition returns true
unsigned int findLastIndexWhere(const std::function<bool (unsigned int)>& condition, unsigned int n_values)
{
    unsigned int first_idx = 0;
    unsigned int length = n_values;

    while (length > 0)
    {
        // Compute midpoint
        unsigned int half_length = length >> 1;
        unsigned int middle_idx = first_idx + half_length;

        if (condition(middle_idx)) // Does the condition still hold at the midpoint?
        {
            // Continue search in the second half
            first_idx = middle_idx + 1;
            length -= half_length + 1;
        }
        else
        {
            // Continue search in the first half
            length = half_length;
        }
    }

    return clamp(first_idx-1, 0u, n_values-2);
}

} // Impact
