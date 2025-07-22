#pragma once

#include "Vector3f.h"
#include "Vector4f.h"
#include "Matrix4f.h"
#include <iostream>

class Quaternion
{
public:
	float w, x, y, z;

	// ctors
	Quaternion();																		// initializes identity quaternion
	Quaternion(float w, float x, float y, float z);										// initializes quaternion with four floats
	Quaternion(float angle, Vector3f axis);

	// operation overload
	Quaternion operator+(const Quaternion& other) const;									// quaternion addition
	Quaternion operator-(const Quaternion& other) const;									// quaternion subtraction
	Quaternion operator*(const Quaternion& other) const;									// quaternion multiplication
	Quaternion operator*(float scalar) const;												// quaternion multiplication with a scalar
	Quaternion operator/(float scalar) const;												// quaternion division by a scalar


	// assign overload
	Quaternion& operator+=(const Quaternion& other);										// quaternion addition assignment
	Quaternion& operator-=(const Quaternion& other);										// quaternion subtraction assignment
	Quaternion& operator*=(const Quaternion& other);										// quaternion multiplication assignment
	Quaternion& operator*=(float scalar);													// quaternion multiplication assignment with a scalar
	Quaternion& operator/=(float scalar);													// quaternion division assignment by a scalar

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& q);

	Quaternion conjugate() const;															// returns the conjugate of the quaternion
	Quaternion inverse() const;																// returns the inverse of the quaternion

	Matrix4f getMatrix() const;																// converts the quaternion to a 4x4 matrix

	Quaternion& normalize();																// normalizes the quaternion
	Quaternion getNormalized() const;														// returns a normalized copy of the quaternion
};
