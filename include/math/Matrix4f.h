#pragma once

#include "Vector4f.h"
#include <iostream>
#include "Vector3f.h"

class Matrix4f
{
public:
	float a11, a21, a31, a41, a12, a22, a32, a42, a13, a23, a33, a43, a14, a24, a34, a44;

	Matrix4f();														// initializes identity matrix
	Matrix4f(float a11, float a21, float a31, float a41,
		float a12, float a22, float a32, float a42,
		float a13, float a23, float a33, float a43,
		float a14, float a24, float a34, float a44);
	Matrix4f(Vector4f i, Vector4f j, Vector4f k, Vector4f l);
	Matrix4f(float x);												// initializes a matrix with the same number in each position
	Matrix4f(Vector3f translation);
	
	
	// operation overload
	Matrix4f operator+ (const Matrix4f& other) const;
	Matrix4f operator- (const Matrix4f& other) const;
	Matrix4f operator* (float scalar) const;
	Vector4f operator* (const Vector4f v) const;
	Matrix4f operator* (const Matrix4f& other) const;
	Matrix4f operator/ (float scalar) const;


	// assign overload
	Matrix4f& operator+=(const Matrix4f& other);
	Matrix4f& operator-=(const Matrix4f& other);
	Matrix4f& operator*=(float scalar);
	Matrix4f& operator*=(const Matrix4f& other);
	Matrix4f& operator/=(float scalar);

	bool isInvertible() const;
	float det() const;
	float cofactor(int row, int col) const;
	Matrix4f adjugate() const;
	Matrix4f inverse() const;
	Matrix4f& transpose();
	Matrix4f getTranspose() const;
	Matrix4f& projection(float left, float right, float bottom, float top, float near, float far);
	Matrix4f& lookAt(const Vector3f& eye, const Vector3f& center, const Vector3f& up);

	// output overload
	friend std::ostream& operator<<(std::ostream& os, const Matrix4f& M);
};
