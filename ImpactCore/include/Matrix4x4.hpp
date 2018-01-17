#pragma once
#include "precision.hpp"

namespace Impact {

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

} // Impact
