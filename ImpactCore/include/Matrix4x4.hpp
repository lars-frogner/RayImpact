#pragma once
#include "precision.hpp"

namespace Impact {

// Matrix4x4 declarations

class Matrix4x4 {

public:
    // Matrix elements
    imp_float a11, a12, a13, a14,
              a21, a22, a23, a24,
              a31, a32, a33, a34,
              a41, a42, a43, a44;

    Matrix4x4();

    Matrix4x4(imp_float a11, imp_float a12, imp_float a13, imp_float a14,
              imp_float a21, imp_float a22, imp_float a23, imp_float a24,
              imp_float a31, imp_float a32, imp_float a33, imp_float a34,
              imp_float a41, imp_float a42, imp_float a43, imp_float a44);

    Matrix4x4(const imp_float elements[16]);

    bool operator==(const Matrix4x4& other) const;
    bool operator!=(const Matrix4x4& other) const;

    bool isIdentity() const;

    Matrix4x4 operator*(const Matrix4x4& other) const;
    Matrix4x4& operator*=(const Matrix4x4& other);

    imp_float determinant() const;

    Matrix4x4 transposed() const;

    Matrix4x4 inverted() const;
};

// Matrix4x4 inline method definitions

inline Matrix4x4::Matrix4x4()
	: a11(1), a12(0), a13(0), a14(0),
	  a21(0), a22(1), a23(0), a24(0),
	  a31(0), a32(0), a33(1), a34(0),
	  a41(0), a42(0), a43(0), a44(1)
{}

inline Matrix4x4::Matrix4x4(imp_float a11, imp_float a12, imp_float a13, imp_float a14,
							imp_float a21, imp_float a22, imp_float a23, imp_float a24,
							imp_float a31, imp_float a32, imp_float a33, imp_float a34,
							imp_float a41, imp_float a42, imp_float a43, imp_float a44)
    : a11(a11), a12(a12), a13(a13), a14(a14),
      a21(a21), a22(a22), a23(a23), a24(a24),
      a31(a31), a32(a32), a33(a33), a34(a34),
      a41(a41), a42(a42), a43(a43), a44(a44)
{}

inline Matrix4x4::Matrix4x4(const imp_float elements[16])
    : a11(elements[0]),  a12(elements[1]),  a13(elements[2]),  a14(elements[3]),
      a21(elements[4]),  a22(elements[5]),  a23(elements[6]),  a24(elements[7]),
      a31(elements[8]),  a32(elements[9]),  a33(elements[10]), a34(elements[11]),
      a41(elements[12]), a42(elements[13]), a43(elements[14]), a44(elements[15])
{}

inline bool Matrix4x4::operator==(const Matrix4x4& other) const
{
    return a11 == other.a11 && a12 == other.a12 && a13 == other.a13 && a14 == other.a14 &&
           a21 == other.a21 && a22 == other.a22 && a23 == other.a23 && a24 == other.a24 &&
           a31 == other.a31 && a32 == other.a32 && a33 == other.a33 && a34 == other.a34 &&
           a41 == other.a41 && a42 == other.a42 && a43 == other.a43 && a44 == other.a44;
}

inline bool Matrix4x4::operator!=(const Matrix4x4& other) const
{
    return !(*this == other);
}

inline bool Matrix4x4::isIdentity() const
{
    return a11 == 1 && a12 == 0 && a13 == 0 && a14 == 0 &&
           a21 == 0 && a22 == 1 && a23 == 0 && a24 == 0 &&
           a31 == 0 && a32 == 0 && a33 == 1 && a34 == 0 &&
           a41 == 0 && a42 == 0 && a43 == 0 && a44 == 1;
}

inline Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
	Matrix4x4 result;

	result.a11 = other.a11*a11 + other.a21*a12 + other.a31*a13 + other.a41*a14;
	result.a12 = other.a12*a11 + other.a22*a12 + other.a32*a13 + other.a42*a14;
	result.a13 = other.a13*a11 + other.a23*a12 + other.a33*a13 + other.a43*a14;
	result.a14 = other.a14*a11 + other.a24*a12 + other.a34*a13 + other.a44*a14;
	result.a21 = other.a11*a21 + other.a21*a22 + other.a31*a23 + other.a41*a24;
	result.a22 = other.a12*a21 + other.a22*a22 + other.a32*a23 + other.a42*a24;
	result.a23 = other.a13*a21 + other.a23*a22 + other.a33*a23 + other.a43*a24;
	result.a24 = other.a14*a21 + other.a24*a22 + other.a34*a23 + other.a44*a24;
	result.a31 = other.a11*a31 + other.a21*a32 + other.a31*a33 + other.a41*a34;
	result.a32 = other.a12*a31 + other.a22*a32 + other.a32*a33 + other.a42*a34;
	result.a33 = other.a13*a31 + other.a23*a32 + other.a33*a33 + other.a43*a34;
	result.a34 = other.a14*a31 + other.a24*a32 + other.a34*a33 + other.a44*a34;
	result.a41 = other.a11*a41 + other.a21*a42 + other.a31*a43 + other.a41*a44;
	result.a42 = other.a12*a41 + other.a22*a42 + other.a32*a43 + other.a42*a44;
	result.a43 = other.a13*a41 + other.a23*a42 + other.a33*a43 + other.a43*a44;
	result.a44 = other.a14*a41 + other.a24*a42 + other.a34*a43 + other.a44*a44;

	return result;

	/*
	// For some reason the correct values are not passed on to the Matrix4x4 constructor
	// when using this expreession..
	return Matrix4x4(other.a11*a11 + other.a21*a12 + other.a31*a13 + other.a41*a14,
					 other.a12*a11 + other.a22*a12 + other.a32*a13 + other.a42*a14,
					 other.a13*a11 + other.a23*a12 + other.a33*a13 + other.a43*a14,
					 other.a14*a11 + other.a24*a12 + other.a34*a13 + other.a44*a14,
					 other.a11*a21 + other.a21*a22 + other.a31*a23 + other.a41*a24,
					 other.a12*a21 + other.a22*a22 + other.a32*a23 + other.a42*a24,
					 other.a13*a21 + other.a23*a22 + other.a33*a23 + other.a43*a24,
					 other.a14*a21 + other.a24*a22 + other.a34*a23 + other.a44*a24,
					 other.a11*a31 + other.a21*a32 + other.a31*a33 + other.a41*a34,
					 other.a12*a31 + other.a22*a32 + other.a32*a33 + other.a42*a34,
					 other.a13*a31 + other.a23*a32 + other.a33*a33 + other.a43*a34,
					 other.a14*a31 + other.a24*a32 + other.a34*a33 + other.a44*a34,
					 other.a11*a41 + other.a21*a42 + other.a31*a43 + other.a41*a44,
					 other.a12*a41 + other.a22*a42 + other.a32*a43 + other.a42*a44,
					 other.a13*a41 + other.a23*a42 + other.a33*a43 + other.a43*a44,
					 other.a14*a41 + other.a24*a42 + other.a34*a43 + other.a44*a44);
	*/
}

inline Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
{
    *this = (*this)*(other);
    return *this;
}

inline imp_float Matrix4x4::determinant() const
{
    return a14*a23*a32*a41 - a13*a24*a32*a41 -
           a14*a22*a33*a41 + a12*a24*a33*a41 +
           a13*a22*a34*a41 - a12*a23*a34*a41 -
           a14*a23*a31*a42 + a13*a24*a31*a42 +
           a14*a21*a33*a42 - a11*a24*a33*a42 -
           a13*a21*a34*a42 + a11*a23*a34*a42 +
           a14*a22*a31*a43 - a12*a24*a31*a43 -
           a14*a21*a32*a43 + a11*a24*a32*a43 +
           a12*a21*a34*a43 - a11*a22*a34*a43 -
           a13*a22*a31*a44 + a12*a23*a31*a44 +
           a13*a21*a32*a44 - a11*a23*a32*a44 -
           a12*a21*a33*a44 + a11*a22*a33*a44;
}

inline Matrix4x4 Matrix4x4::transposed() const
{
    return Matrix4x4(a11, a21, a31, a41,
                     a12, a22, a32, a42,
                     a13, a23, a33, a43,
                     a14, a24, a43, a44);
}

} // Impact
