#include "math/Quaternion.h"
#include <cmath>

// ctors
Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}

Quaternion::Quaternion(float w, float x, float y, float z) : w(w), x(x), y(y), z(z) {}

Quaternion::Quaternion(float angle, Vector3f axis) 
{
	axis.normalize();
    float halfAngle = angle * 0.5f;
    float sinHalf = sin(halfAngle);

    w = cos(halfAngle);
    x = axis.x * sinHalf;
    y = axis.y * sinHalf;
    z = axis.z * sinHalf;
}

// Operations
Quaternion Quaternion::operator+(const Quaternion& other) const 
{
    return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
}

Quaternion Quaternion::operator-(const Quaternion& other) const 
{
    return Quaternion(w - other.w, x - other.x, y - other.y, z - other.z);
}

Quaternion Quaternion::operator*(const Quaternion& other) const 
{
    return Quaternion(
        w * other.w - x * other.x - y * other.y - z * other.z,
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w
    );
}

Quaternion Quaternion::operator*(float scalar) const 
{
    return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
}

Quaternion Quaternion::operator/(float scalar) const 
{
    return Quaternion(w / scalar, x / scalar, y / scalar, z / scalar);
}

// Assignment operations
Quaternion& Quaternion::operator+=(const Quaternion& other) 
{
    w += other.w;
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Quaternion& Quaternion::operator-=(const Quaternion& other) 
{
    w -= other.w;
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}

Quaternion& Quaternion::operator*=(const Quaternion& other) 
{
    *this = *this * other;
    return *this;
}

Quaternion& Quaternion::operator*=(float scalar) 
{
    w *= scalar;
    x *= scalar;
    y *= scalar;
    z *= scalar;
    return *this;
}

Quaternion& Quaternion::operator/=(float scalar) 
{
    w /= scalar;
    x /= scalar;
    y /= scalar;
    z /= scalar;
    return *this;
}

// Utility functions
Quaternion Quaternion::conjugate() const 
{
    return Quaternion(w, -x, -y, -z);
}

Quaternion Quaternion::inverse() const 
{
    float magnitude = w * w + x * x + y * y + z * z;
    if (magnitude == 0.0f) return Quaternion(); // Return identity if zero magnitude

    Quaternion conj = conjugate();
    return conj / magnitude;
}

Matrix4f Quaternion::getMatrix() const 
{
    float xx = x * x;
    float yy = y * y;
    float zz = z * z;
    float xy = x * y;
    float xz = x * z;
    float yz = y * z;
    float wx = w * x;
    float wy = w * y;
    float wz = w * z;

    return Matrix4f(
        1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
        2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
        2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}

Quaternion& Quaternion::normalize() 
{
    float magnitude = sqrt(w * w + x * x + y * y + z * z);
    if (magnitude == 0.0f) 
    {
        *this = Quaternion(); // Return identity if zero magnitude
        return *this;
    }

    w /= magnitude;
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
    return *this;
}

Quaternion Quaternion::getNormalized() const 
{
    Quaternion result = *this;
    return result.normalize();
}

// Output stream operator
std::ostream& operator<<(std::ostream& os, const Quaternion& q) 
{
    os << "(" << q.w << ", " << q.x << ", " << q.y << ", " << q.z << ")";
    return os;
}