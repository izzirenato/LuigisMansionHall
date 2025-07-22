#pragma once

#include <iostream>

class Matrix3f;

class Vector3f
{
public:
    float x, y, z;

    // ctor overload
    Vector3f();                                 // initializes vector with all zeros
    Vector3f(float x, float y, float z);        // creates a vector with three floats
    Vector3f(float value);                      // initializes vector with the same number

    // operation overload
    Vector3f operator+(const Vector3f& other) const;
    Vector3f operator-(const Vector3f& other) const;
    Vector3f operator*(float scalar) const;
    Vector3f operator/(float scalar) const;

    // assign overload
    Vector3f& operator+=(const Vector3f& other);
    Vector3f& operator-=(const Vector3f& other);
    Vector3f& operator*=(float scalar);
    Vector3f& operator*=(const Matrix3f& other);
    Vector3f& operator/=(float scalar);

    float dot(const Vector3f& other) const;
    Vector3f cross(const Vector3f& other) const;     

    float magnitude() const;
    Vector3f getNormalized() const;
    Vector3f& normalize();

    // output overload
    friend std::ostream& operator<<(std::ostream& os, const Vector3f& v);
};

