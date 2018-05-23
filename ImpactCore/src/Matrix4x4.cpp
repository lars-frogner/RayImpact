#include "Matrix4x4.hpp"
#include "error.hpp"

namespace Impact {

// Matrix4x4 method definitions

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
