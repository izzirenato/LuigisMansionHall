#include "math/Vector3f.h"
#include "math/Matrix3f.h"
#include <iostream>

// ctor
Vector3f::Vector3f() : x(0), y(0), z(0) {}
Vector3f::Vector3f(float x, float y, float z) : x(x), y(y), z(z) {}
Vector3f::Vector3f(float value) : x(value), y(value), z(value) {}

// overload +
Vector3f Vector3f::operator+(const Vector3f& other) const 
{
    return Vector3f(x + other.x, y + other.y, z + other.z);
}

// overload -
Vector3f Vector3f::operator-(const Vector3f& other) const 
{
    return Vector3f(x - other.x, y - other.y, z - other.z);
}

// overload * multiply with a scalar
Vector3f Vector3f::operator*(float scalar) const 
{
    return Vector3f(x * scalar, y * scalar, z * scalar);
}

// overload / division with a scalar 
Vector3f Vector3f::operator/(float scalar) const 
{
	float factor = 1.0f / scalar;
    return Vector3f(x * factor, y * factor, z * factor);
}

// dot Product
float Vector3f::dot(const Vector3f& other) const 
{
    return (x * other.x) + (y * other.y) + (z * other.z);
}

// cross Product
Vector3f Vector3f::cross(const Vector3f& other) const 
{
    return Vector3f(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    );
}

// Overload <<
std::ostream& operator<<(std::ostream& os, const Vector3f& v) 
{
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}

// Overload +=
Vector3f& Vector3f::operator+=(const Vector3f& other) 
{
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

// Overload -=
Vector3f& Vector3f::operator-=(const Vector3f& other) 
{
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

// Overload *=
Vector3f& Vector3f::operator*=(float scalar) 
{
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Vector3f& Vector3f::operator*=(const Matrix3f& other)
{
    Vector3f r1 = Vector3f(other.a11, other.a12, other.a13);
    Vector3f r2 = Vector3f(other.a21, other.a22, other.a23);
    Vector3f r3 = Vector3f(other.a31, other.a32, other.a33);

    x = r1.dot(*this);
    y = r2.dot(*this);
    z = r3.dot(*this);

    return *this;
}

// Overload /=
Vector3f& Vector3f::operator/=(float scalar) 
{
	float factor = 1.0f / scalar;
	x *= factor;
	y *= factor;
	z *= factor;
    return *this;
}

// Vector length
float Vector3f::magnitude() const 
{
    return std::sqrt(x * x + y * y + z * z);
}

// Copy of normalized vector
Vector3f Vector3f::getNormalized() const 
{
    float mag = magnitude();
    if (mag != 0) 
    {
		float factor = 1.0f / mag;
		return Vector3f(x * factor, y * factor, z * factor);
    }
	return Vector3f(0, 0, 0); // Return zero vector if magnitude is zero
}

// normalizes a vector
Vector3f& Vector3f::normalize() 
{
    float mag = magnitude();
    if (mag != 0) 
    {
		float factor = 1.0f / mag;
        *this *= factor;
    }
    return *this;
}