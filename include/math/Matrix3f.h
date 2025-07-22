#pragma once

#include <iostream>

// Forward declaration instead of #include "Vector3f.h"
class Vector3f;

class Matrix3f
{
public:
   float a11, a21, a31, a12, a22, a32, a13, a23, a33;

   Matrix3f();                                                                                  // initializes identity matrix
   Matrix3f(float a, float b, float c, float d, float e, float f, float g, float h, float i);
   Matrix3f(Vector3f i, Vector3f j, Vector3f k);
   Matrix3f(float x);                                                                           // initializes a matrix with the same number in each position

   // operation overload
   Matrix3f operator+ (const Matrix3f& other) const;
   Matrix3f operator- (const Matrix3f& other) const;
   Matrix3f operator* (float scalar) const;
   Vector3f operator* (const Vector3f v) const;
   Matrix3f operator* (const Matrix3f& other) const;
   Matrix3f operator/ (float scalar) const;

   // assign overload
   Matrix3f& operator+=(const Matrix3f& other);
   Matrix3f& operator-=(const Matrix3f& other);
   Matrix3f& operator*=(float scalar);
   Matrix3f& operator*=(const Matrix3f& other);
   Matrix3f& operator/=(float scalar);

   bool isInvertible() const;
   float det() const;
   float cofactor(int row, int col) const;
   Matrix3f adjugate() const;
   Matrix3f inverse() const;
   Matrix3f& transpose();
   Matrix3f getTranspose() const;

   // output overload
   friend std::ostream& operator<<(std::ostream& os, const Matrix3f& M);
};
