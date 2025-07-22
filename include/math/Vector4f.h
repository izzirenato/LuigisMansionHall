#pragma once

#include <iostream>

class Matrix4f;

class Vector4f
{
public:
	float x, y, z, w;

	// ctor overload
	Vector4f();												// initializes vector with all zeros
	Vector4f(float x, float y, float z, float w);			// creates a vector with these four floats
	Vector4f(float value);									// initializes vector with the same number

	// operation overload
	Vector4f operator+(const Vector4f& other) const;
	Vector4f operator-(const Vector4f& other) const;
	Vector4f operator*(float scalar) const;
	Vector4f operator/(float scalar) const;

	// assign overload
	Vector4f& operator+=(const Vector4f& other);
	Vector4f& operator-=(const Vector4f& other);
	Vector4f& operator*=(float scalar);
	Vector4f& operator*=(const Matrix4f& other);
	Vector4f& operator/=(float scalar);


	float dot(const Vector4f& other) const;

	float magnitude() const;
	Vector4f getNormalized() const;
	Vector4f& normalize();

	// output overload
	friend std::ostream& operator<<(std::ostream& os, const Vector4f& v);
};
