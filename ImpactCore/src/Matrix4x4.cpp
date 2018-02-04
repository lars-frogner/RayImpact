#include "Matrix4x4.hpp"
#include "error.hpp"

namespace Impact {

Matrix4x4::Matrix4x4()
    : a11(1), a12(0), a13(0), a14(0),
      a21(0), a22(1), a23(0), a24(0),
      a31(0), a32(0), a33(1), a34(0),
      a41(0), a42(0), a43(0), a44(1)
{}

Matrix4x4::Matrix4x4(imp_float a11, imp_float a12, imp_float a13, imp_float a14,
                     imp_float a21, imp_float a22, imp_float a23, imp_float a24,
                     imp_float a31, imp_float a32, imp_float a33, imp_float a34,
                     imp_float a41, imp_float a42, imp_float a43, imp_float a44)
    : a11(a11), a12(a12), a13(a13), a14(a14),
      a21(a21), a22(a22), a23(a23), a24(a24),
      a31(a31), a32(a32), a33(a33), a34(a34),
      a41(a41), a42(a42), a43(a43), a44(a44)
{}

Matrix4x4::Matrix4x4(const imp_float elements[16])
    : a11(elements[0]),  a12(elements[1]),  a13(elements[2]),  a14(elements[3]),
      a21(elements[4]),  a22(elements[5]),  a23(elements[6]),  a24(elements[7]),
      a31(elements[8]),  a32(elements[9]),  a33(elements[10]), a34(elements[11]),
      a41(elements[12]), a42(elements[13]), a43(elements[14]), a44(elements[15])
{}

bool Matrix4x4::operator==(const Matrix4x4& other) const
{
    return a11 == other.a11 && a12 == other.a12 && a13 == other.a13 && a14 == other.a14 &&
           a21 == other.a21 && a22 == other.a22 && a23 == other.a23 && a24 == other.a24 &&
           a31 == other.a31 && a32 == other.a32 && a33 == other.a33 && a34 == other.a34 &&
           a41 == other.a41 && a42 == other.a42 && a43 == other.a43 && a44 == other.a44;
}

bool Matrix4x4::operator!=(const Matrix4x4& other) const
{
    return !(*this == other);
}

bool Matrix4x4::isIdentity() const
{
    return a11 == 1 && a12 == 0 && a13 == 0 && a14 == 0 &&
           a21 == 0 && a22 == 1 && a23 == 0 && a24 == 0 &&
           a31 == 0 && a32 == 0 && a33 == 1 && a34 == 0 &&
           a41 == 0 && a42 == 0 && a43 == 0 && a44 == 1;
}

Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
{
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
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
{
    *this = (*this)*(other);
    return *this;
}

imp_float Matrix4x4::determinant() const
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

Matrix4x4 Matrix4x4::transposed() const
{
    return Matrix4x4(a11, a21, a31, a41,
                     a12, a22, a32, a42,
                     a13, a23, a33, a43,
                     a14, a24, a43, a44);
}

Matrix4x4 Matrix4x4::inverted() const
{
    imp_float b11, b12, b13, b14,
              b21, b22, b23, b24,
              b31, b32, b33, b34,
              b41, b42, b43, b44;

    b11 = a22*a33*a44 -
          a22*a34*a43 -
          a32*a23*a44 +
          a32*a24*a43 +
          a42*a23*a34 -
          a42*a24*a33;

    b21 = -a21*a33*a44 +
           a21*a34*a43 +
           a31*a23*a44 -
           a31*a24*a43 -
           a41*a23*a34 +
           a41*a24*a33;

    b31 = a21*a32*a44 -
          a21*a34*a42 -
          a31*a22*a44 +
          a31*a24*a42 +
          a41*a22*a34 -
          a41*a24*a32;

    b41 = -a21*a32*a43 +
           a21*a33*a42 +
           a31*a22*a43 -
           a31*a23*a42 -
           a41*a22*a33 +
           a41*a23*a32;

    b12 = -a12*a33*a44 +
           a12*a34*a43 +
           a32*a13*a44 -
           a32*a14*a43 -
           a42*a13*a34 +
           a42*a14*a33;

    b22 = a11*a33*a44 -
          a11*a34*a43 -
          a31*a13*a44 +
          a31*a14*a43 +
          a41*a13*a34 -
          a41*a14*a33;

    b32 = -a11*a32*a44 +
           a11*a34*a42 +
           a31*a12*a44 -
           a31*a14*a42 -
           a41*a12*a34 +
           a41*a14*a32;

    b42 = a11*a32*a43 -
          a11*a33*a42 -
          a31*a12*a43 +
          a31*a13*a42 +
          a41*a12*a33 -
          a41*a13*a32;

    b13 = a12*a23*a44 -
          a12*a24*a43 -
          a22*a13*a44 +
          a22*a14*a43 +
          a42*a13*a24 -
          a42*a14*a23;

    b23 = -a11*a23*a44 +
           a11*a24*a43 +
           a21*a13*a44 -
           a21*a14*a43 -
           a41*a13*a24 +
           a41*a14*a23;

    b33 = a11*a22*a44 -
          a11*a24*a42 -
          a21*a12*a44 +
          a21*a14*a42 +
          a41*a12*a24 -
          a41*a14*a22;

    b43 = -a11*a22*a43 +
           a11*a23*a42 +
           a21*a12*a43 -
           a21*a13*a42 -
           a41*a12*a23 +
           a41*a13*a22;

    b14 = -a12*a23*a34 +
           a12*a24*a33 +
           a22*a13*a34 -
           a22*a14*a33 -
           a32*a13*a24 +
           a32*a14*a23;

    b24 = a11*a23*a34 -
          a11*a24*a33 -
          a21*a13*a34 +
          a21*a14*a33 +
          a31*a13*a24 -
          a31*a14*a23;

    b34 = -a11*a22*a34 +
           a11*a24*a32 +
           a21*a12*a34 -
           a21*a14*a32 -
           a31*a12*a24 +
           a31*a14*a22;

    b44 = a11*a22*a33 -
          a11*a23*a32 -
          a21*a12*a33 +
          a21*a13*a32 +
          a31*a12*a23 -
          a31*a13*a22;

    imp_float inv_det = a11*b11 + a12*b21 + a13*b31 + a14*b41;

    // Make sure matrix is invertible (non-zero determinant)
    imp_assert(inv_det != 0);

    inv_det = 1.0f/inv_det;

    return Matrix4x4(b11*inv_det, b12*inv_det, b13*inv_det, b14*inv_det,
                     b21*inv_det, b22*inv_det, b23*inv_det, b24*inv_det,
                     b31*inv_det, b32*inv_det, b33*inv_det, b34*inv_det,
                     b41*inv_det, b42*inv_det, b43*inv_det, b44*inv_det);
}

} // Impact
