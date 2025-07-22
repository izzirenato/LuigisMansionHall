#include "math/Vector4f.h"
#include "math/Matrix4f.h"
#include <iostream>

// ctor
Vector4f::Vector4f() : x(0), y(0), z(0), w(0) {}
Vector4f::Vector4f(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
Vector4f::Vector4f(float value) : x(value), y(value), z(value), w(value) {}

// overload +
Vector4f Vector4f::operator+(const Vector4f& other) const
{
	return Vector4f(x + other.x, y + other.y, z + other.z, w + other.w);
}

// overload -
Vector4f Vector4f::operator-(const Vector4f& other) const
{
	return Vector4f(x - other.x, y - other.y, z - other.z, w - other.w);
}

// overload * multiply with a scalar
Vector4f Vector4f::operator*(float scalar) const
{
	return Vector4f(x * scalar, y * scalar, z * scalar, w * scalar);
}

// overload / division with a scalar
Vector4f Vector4f::operator/(float scalar) const
{
	return Vector4f(x / scalar, y / scalar, z / scalar, w / scalar);
}

// dot Product
float Vector4f::dot(const Vector4f& other) const
{
	return (x * other.x) + (y * other.y) + (z * other.z) + (w * other.w);
}

// Overload <<
std::ostream& operator<<(std::ostream& os, const Vector4f& v)
{
	os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
	return os;
}

// Overload +=
Vector4f& Vector4f::operator+=(const Vector4f& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;
	return *this;
}

// Overload -=
Vector4f& Vector4f::operator-=(const Vector4f& other)
{
	x -= other.x;
	y -= other.y;
	z -= other.z;
	w -= other.w;
	return *this;
}

// Overload *=
Vector4f& Vector4f::operator*=(float scalar)
{
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

Vector4f& Vector4f::operator*=(const Matrix4f& other)
{
	Vector4f r1 = Vector4f(other.a11, other.a12, other.a13, other.a14);
	Vector4f r2 = Vector4f(other.a21, other.a22, other.a23, other.a24);
	Vector4f r3 = Vector4f(other.a31, other.a32, other.a33, other.a34);
	Vector4f r4 = Vector4f(other.a41, other.a42, other.a43, other.a44);
	x = r1.dot(*this);
	y = r2.dot(*this);
	z = r3.dot(*this);
	w = r4.dot(*this);
	return *this;
}

// Overload /=
Vector4f& Vector4f::operator/=(float scalar)
{
	x /= scalar;
	y /= scalar;
	z /= scalar;
	w /= scalar;
	return *this;
}

// magnitude
float Vector4f::magnitude() const
{
	return sqrt(x * x + y * y + z * z + w * w);
}

// getNormalized
Vector4f Vector4f::getNormalized() const
{
	float mag = magnitude();
	if (mag == 0) return Vector4f(0, 0, 0, 0);
	return Vector4f(x / mag, y / mag, z / mag, w / mag);
}

// normalize
Vector4f& Vector4f::normalize()
{
	float mag = magnitude();
	if (mag != 0) 
	{
		*this /= mag;
	}
	return *this;
}
