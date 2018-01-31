#pragma once
#include "precision.hpp"
#include "math.hpp"
#include "string_util.hpp"
#include "error.hpp"
#include <cmath>
#include <algorithm>
#include <ostream>

namespace Impact {

// ErrorFloat declarations

class ErrorFloat {

friend ErrorFloat sqrt(const ErrorFloat& float_with_error);
friend ErrorFloat abs(const ErrorFloat& float_with_error);

friend std::ostream& operator<<(std::ostream& stream, const ErrorFloat& float_with_error);

private:

    imp_float value;
    imp_float upper_bound;
    imp_float lower_bound;

    #ifndef NDEBUG
    long double precise_value;
    #endif

public:

    ErrorFloat();

    ErrorFloat(imp_float value);
    
    ErrorFloat(imp_float value, imp_float error);

    ErrorFloat(const ErrorFloat& other);

    explicit operator imp_float() const;

    ErrorFloat& operator=(const ErrorFloat& other);

    ErrorFloat operator+(const ErrorFloat& other) const;
    ErrorFloat operator-(const ErrorFloat& other) const;
    ErrorFloat operator*(const ErrorFloat& other) const;
    ErrorFloat operator/(const ErrorFloat& other) const;
    
    ErrorFloat operator-() const;
    
    ErrorFloat& operator+=(const ErrorFloat& other);
    ErrorFloat& operator-=(const ErrorFloat& other);
    ErrorFloat& operator*=(const ErrorFloat& other);
    ErrorFloat& operator/=(const ErrorFloat& other);

    bool operator==(const ErrorFloat& other) const;
    bool operator!=(const ErrorFloat& other) const;
    bool operator<(const ErrorFloat& other) const;
    bool operator<=(const ErrorFloat& other) const;
    bool operator>(const ErrorFloat& other) const;
    bool operator>=(const ErrorFloat& other) const;

    imp_float upperBound() const;
    imp_float lowerBound() const;

    imp_float absoluteError() const;
    
    #ifndef NDEBUG
    imp_float relativeError() const;
    long double preciseValue() const;
    #endif

    void verifyBounds() const;
};

// Functions on ErrorFloat objects

inline ErrorFloat sqrt(const ErrorFloat& float_with_error)
{
    ErrorFloat result;

    result.value = std::sqrt(float_with_error.value);
    
    #ifndef NDEBUG
    result.precise_value = std::sqrt(float_with_error.precise_value);
    #endif

    result.lower_bound = closestLowerFloat(std::sqrt(float_with_error.lower_bound));
    result.upper_bound = closestHigherFloat(std::sqrt(float_with_error.upper_bound));

    result.verifyBounds();

    return result;
}

inline ErrorFloat abs(const ErrorFloat& float_with_error)
{
    if (float_with_error.lower_bound >= 0.0)
    {
        return float_with_error;
    }
    else if (float_with_error.upper_bound <= 0.0)
    {
        return -float_with_error;
    }
    else
    {
        ErrorFloat result;

        result.value = std::abs(float_with_error.value);
    
        #ifndef NDEBUG
        result.precise_value = std::abs(float_with_error.precise_value);
        #endif

        result.lower_bound = 0.0;
        result.upper_bound = std::max(-float_with_error.lower_bound, float_with_error.upper_bound);
        
        result.verifyBounds();

        return result;
    }
}

inline std::ostream& operator<<(std::ostream& stream, const ErrorFloat& float_with_error)
{
    stream << formatString("{value = %f, range = [%f, %f]",
                           float_with_error.value,
                           float_with_error.lower_bound,
                           float_with_error.upper_bound);
    
    #ifndef NDEBUG
    stream << formatString(", precise = %f", float_with_error.precise_value);
    #endif

    stream << "}";

    return stream;
}

inline ErrorFloat operator+(imp_float value, const ErrorFloat& float_with_error)
{
    return ErrorFloat(value) + float_with_error;
}

inline ErrorFloat operator-(imp_float value, const ErrorFloat& float_with_error)
{
    return ErrorFloat(value) - float_with_error;
}

inline ErrorFloat operator*(imp_float value, const ErrorFloat& float_with_error)
{
    return ErrorFloat(value)*float_with_error;
}

inline ErrorFloat operator/(imp_float value, const ErrorFloat& float_with_error)
{
    return ErrorFloat(value)/float_with_error;
}

// Solves the quadratic equation a*x^2 + b*x + c = 0 for the solutions x1 (smallest) and x2 (largest) if they exist
inline bool solveQuadraticEquation(const ErrorFloat& a,
                                   const ErrorFloat& b,
                                   const ErrorFloat& c,
                                   ErrorFloat* x1,
                                   ErrorFloat* x2)
{
    imp_assert(x1 && x2);
    imp_assert(a != 0);

    const ErrorFloat& discriminant = b*b - ErrorFloat(4.0)*a*c;

    if (discriminant < 0.0)
        return false;

    const ErrorFloat& sqrt_discriminant = sqrt(discriminant);

    const ErrorFloat& q = (b < 0.0)? ErrorFloat(-0.5)*(b - sqrt_discriminant) : ErrorFloat(-0.5)*(b + sqrt_discriminant);

    *x1 = q/a;
    *x2 = c/q;

    if (*x1 > *x2) std::swap(*x1, *x2);

    return true;
}

// ErrorFloat method implementations

inline ErrorFloat::ErrorFloat()
{}

inline ErrorFloat::ErrorFloat(imp_float value)
    : value(value), lower_bound(value), upper_bound(value)
{
    #ifndef NDEBUG
    precise_value = static_cast<long double>(value);
    #endif
}

inline ErrorFloat::ErrorFloat(imp_float value, imp_float error)
    : value(value),
      lower_bound(closestLowerFloat(value - error)),
      upper_bound(closestHigherFloat(value + error))
{

    #ifndef NDEBUG
    precise_value = static_cast<long double>(value);
    #endif
    
    verifyBounds();
}

inline ErrorFloat::ErrorFloat(const ErrorFloat& other)
{
    other.verifyBounds();

    value = other.value;
    
    #ifndef NDEBUG
    precise_value = other.precise_value;
    #endif

    lower_bound = other.lower_bound;
    upper_bound = other.upper_bound;
}

inline ErrorFloat::operator imp_float() const
{
    return value;
}

inline ErrorFloat& ErrorFloat::operator=(const ErrorFloat& other)
{
    other.verifyBounds();

    if (&other != this)
    {
        value = other.value;
    
        #ifndef NDEBUG
        precise_value = other.precise_value;
        #endif

        lower_bound = other.lower_bound;
        upper_bound = other.upper_bound;
    }

    return *this;
}

inline ErrorFloat ErrorFloat::operator+(const ErrorFloat& other) const
{
    ErrorFloat result;

    result.value = value + other.value;
    
    #ifndef NDEBUG
    result.precise_value = precise_value + other.precise_value;
    #endif

    result.lower_bound = closestLowerFloat(lower_bound + other.lower_bound);
    result.upper_bound = closestHigherFloat(upper_bound + other.upper_bound);

    result.verifyBounds();

    return result;
}

inline ErrorFloat ErrorFloat::operator-(const ErrorFloat& other) const
{
    ErrorFloat result;

    result.value = value - other.value;
    
    #ifndef NDEBUG
    result.precise_value = precise_value - other.precise_value;
    #endif
    
    result.lower_bound = closestLowerFloat(lower_bound - other.upper_bound);
    result.upper_bound = closestHigherFloat(upper_bound - other.lower_bound);

    result.verifyBounds();

    return result;
}

inline ErrorFloat ErrorFloat::operator*(const ErrorFloat& other) const
{
    ErrorFloat result;

    result.value = value*other.value;
    
    #ifndef NDEBUG
    result.precise_value = precise_value*other.precise_value;
    #endif
    
    imp_float product_1 = lower_bound*other.lower_bound;
    imp_float product_2 = lower_bound*other.upper_bound;
    imp_float product_3 = upper_bound*other.lower_bound;
    imp_float product_4 = upper_bound*other.upper_bound;

    result.lower_bound = closestLowerFloat(std::min(std::min(product_1, product_2), std::min(product_3, product_4)));
    result.upper_bound = closestHigherFloat(std::max(std::max(product_1, product_2), std::max(product_3, product_4)));

    result.verifyBounds();

    return result;
}

inline ErrorFloat ErrorFloat::operator/(const ErrorFloat& other) const
{
    ErrorFloat result;

    result.value = value/other.value;
    
    #ifndef NDEBUG
    result.precise_value = precise_value/other.precise_value;
    #endif

    if (other.lower_bound < 0.0 && other.upper_bound > 0.0)
    {
        result.lower_bound = -IMP_INFINITY;
        result.upper_bound = IMP_INFINITY;
    }
    else
    {
        imp_float quotient_1 = lower_bound/other.lower_bound;
        imp_float quotient_2 = lower_bound/other.upper_bound;
        imp_float quotient_3 = upper_bound/other.lower_bound;
        imp_float quotient_4 = upper_bound/other.upper_bound;

        result.lower_bound = closestLowerFloat(std::min(std::min(quotient_1, quotient_2), std::min(quotient_3, quotient_4)));
        result.upper_bound = closestHigherFloat(std::max(std::max(quotient_1, quotient_2), std::max(quotient_3, quotient_4)));

        result.verifyBounds();
    }

    return result;
}

inline ErrorFloat ErrorFloat::operator-() const
{
    ErrorFloat result;

    result.value = -value;
    
    #ifndef NDEBUG
    result.precise_value = -precise_value;
    #endif
    
    result.lower_bound = -upper_bound;
    result.upper_bound = -lower_bound;

    return result;
}

inline ErrorFloat& ErrorFloat::operator+=(const ErrorFloat& other)
{
    *this = *this + other;
    return *this;
}

inline ErrorFloat& ErrorFloat::operator-=(const ErrorFloat& other)
{
    *this = *this - other;
    return *this;
}

inline ErrorFloat& ErrorFloat::operator*=(const ErrorFloat& other)
{
    *this = (*this)*other;
    return *this;
}

inline ErrorFloat& ErrorFloat::operator/=(const ErrorFloat& other)
{
    *this = (*this)/other;
    return *this;
}

inline bool ErrorFloat::operator==(const ErrorFloat& other) const
{
    return value == other.value;
}

inline bool ErrorFloat::operator!=(const ErrorFloat& other) const
{
    return !(*this == other);
}

inline bool ErrorFloat::operator<(const ErrorFloat& other) const
{
    return value < other.value;
}

inline bool ErrorFloat::operator<=(const ErrorFloat& other) const
{
    return value <= other.value;
}

inline bool ErrorFloat::operator>(const ErrorFloat& other) const
{
    return value > other.value;
}

inline bool ErrorFloat::operator>=(const ErrorFloat& other) const
{
    return value >= other.value;
}

inline imp_float ErrorFloat::upperBound() const
{
    return upper_bound;
}

inline imp_float ErrorFloat::lowerBound() const
{
    return lower_bound;
}

inline imp_float ErrorFloat::absoluteError() const
{
    return upper_bound - lower_bound;
}

#ifndef NDEBUG
inline imp_float ErrorFloat::relativeError() const
{
    return (imp_float)(std::abs((value - precise_value)/precise_value));
}

inline long double ErrorFloat::preciseValue() const
{
    return precise_value;
}
#endif

inline void ErrorFloat::verifyBounds() const
{
    #ifndef NDEBUG
    if (!std::isinf(lower_bound) && !std::isinf(upper_bound) &&
        !isNaN(lower_bound) && !isNaN(upper_bound))
    {
        imp_assert(lower_bound <= upper_bound);
    }

    if (!std::isinf(value) && !isNaN(value))
    {
        imp_assert(precise_value <= upper_bound);
        imp_assert(precise_value >= lower_bound);
    }
    #endif
}

} // Impact
