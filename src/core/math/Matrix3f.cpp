#include <iostream>

#include "math/Matrix3f.h"
#include "math/Vector3f.h"
//ctor
Matrix3f::Matrix3f()
{
	a11 = 1, a22 = 1, a33 = 1;
	a12 = 0, a13 = 0, a21 = 0, 
	a23 = 0, a31 = 0, a32 = 0;
}

Matrix3f::Matrix3f(Vector3f i, Vector3f j, Vector3f k)
{
	a11 = i.x, a21 = i.y, a31 = i.z;
	a12 = j.x, a22 = j.y, a32 = j.z;
	a13 = k.x, a23 = k.y, a33 = k.z;
}

Matrix3f::Matrix3f(float value) : a11(value), a12(value), a13(value), a21(value), a22(value), a23(value), a31(value), a32(value), a33(value) {}
Matrix3f::Matrix3f(float a, float b, float c, float d, float e, float f, float g, float h, float i) : a11(a), a21(b), a31(c), a12(d), a22(e), a32(f), a13(g), a23(h), a33(i) {}

Matrix3f Matrix3f::operator+(const Matrix3f& other) const
{
	return Matrix3f(a11 + other.a11, a21 + other.a21, a31 + other.a31,
					a12 + other.a12, a22 + other.a22, a32 + other.a32,
					a13 + other.a13, a23 + other.a23, a33 + other.a33);
}

// overload -
Matrix3f Matrix3f::operator-(const Matrix3f& other) const
{
	return Matrix3f(a11 - other.a11, a21 - other.a21, a31 - other.a31,
					a12 - other.a12, a22 - other.a22, a32 - other.a32,
					a13 - other.a13, a23 - other.a23, a33 - other.a33);
}

// overload * multiply with a scalar 
Matrix3f Matrix3f::operator*(float scalar) const
{
	return Matrix3f(a11 * scalar, a21 * scalar, a31 * scalar,
					a12 * scalar, a22 * scalar, a32 * scalar,
					a13 * scalar, a23 * scalar, a33 * scalar);
}

// overload * multiply with a matrix
Matrix3f Matrix3f::operator*(const Matrix3f& other) const
{
	Vector3f r1 = Vector3f(a11, a12, a13);
	Vector3f r2 = Vector3f(a21, a22, a23);
	Vector3f r3 = Vector3f(a31, a32, a33);

	Vector3f c1 = Vector3f(other.a11, other.a21, other.a31);
	Vector3f c2 = Vector3f(other.a12, other.a22, other.a32);
	Vector3f c3 = Vector3f(other.a13, other.a23, other.a33);

	return Matrix3f(r1.dot(c1), r2.dot(c1), r3.dot(c1),
		r1.dot(c2), r2.dot(c2), r3.dot(c2),
		r1.dot(c3), r2.dot(c3), r3.dot(c3));
}

// overload * multiply with a vector
Vector3f Matrix3f::operator*(const Vector3f v) const
{
	Vector3f r1 = Vector3f(a11, a12, a13);
	Vector3f r2 = Vector3f(a21, a22, a23);
	Vector3f r3 = Vector3f(a31, a32, a33);

	return Vector3f(r1.dot(v), r2.dot(v), r3.dot(v));
}

// overload / division with a scalar 
Matrix3f Matrix3f::operator/(float scalar) const
{
	float factor = 1.0f / scalar;
	return Matrix3f(a11 * factor, a21 * factor, a31 * factor,
		a12 * factor, a22 * factor, a32 * factor,
		a13 * factor, a23 * factor, a33 * factor);
}

float Matrix3f::det() const
{
	return a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a12 * a21 * a33 - a11 * a23 * a32 - a13 * a22 * a31;
}

// Overload <<
std::ostream& operator<<(std::ostream& os, const Matrix3f& M)
{
	os << "| " << M.a11 << " " << M.a12 << " " << M.a13 << " |\n"
		<< "| " << M.a21 << " " << M.a22 << " " << M.a23 << " |\n"
		<< "| " << M.a31 << " " << M.a32 << " " << M.a33 << " |\n";
	return os;
}

// Overload +=
Matrix3f& Matrix3f::operator+=(const Matrix3f& other)
{
	a11 += other.a11;
	a12 += other.a12;
	a13 += other.a13;
	a21 += other.a21;
	a22 += other.a22;
	a23 += other.a23;
	a31 += other.a31;
	a32 += other.a32;
	a33 += other.a33;
	return *this;
}

// Overload -=
Matrix3f& Matrix3f::operator-=(const Matrix3f& other)
{
	a11 -= other.a11;
	a12 -= other.a12;
	a13 -= other.a13;
	a21 -= other.a21;
	a22 -= other.a22;
	a23 -= other.a23;
	a31 -= other.a31;
	a32 -= other.a32;
	a33 -= other.a33;
	return *this;
}

// Overload *=
Matrix3f& Matrix3f::operator*=(float scalar)
{
	a11 *= scalar;
	a21 *= scalar;
	a31 *= scalar;
	a12 *= scalar;
	a22 *= scalar;
	a32 *= scalar;
	a13 *= scalar;
	a23 *= scalar;
	a33 *= scalar;
	return *this;
}

Matrix3f& Matrix3f::operator*=(const Matrix3f& other)
{
	Matrix3f temp = *this;

	// Create row vectors from this matrix
	Vector3f r1(temp.a11, temp.a12, temp.a13);
	Vector3f r2(temp.a21, temp.a22, temp.a23);
	Vector3f r3(temp.a31, temp.a32, temp.a33);

	// Create column vectors from the other matrix
	Vector3f c1(other.a11, other.a21, other.a31);
	Vector3f c2(other.a12, other.a22, other.a32);
	Vector3f c3(other.a13, other.a23, other.a33);

	// Compute the dot products for each position in the result matrix
	a11 = r1.dot(c1); a12 = r1.dot(c2); a13 = r1.dot(c3);
	a21 = r2.dot(c1); a22 = r2.dot(c2); a23 = r2.dot(c3);
	a31 = r3.dot(c1); a32 = r3.dot(c2); a33 = r3.dot(c3);

	return *this;
}

// Overload /=
Matrix3f & Matrix3f::operator/=(float scalar)
{
	float factor = 1.0f / scalar;

	a11 *= factor;
	a21 *= factor;
	a31 *= factor;
	a12 *= factor;
	a22 *= factor;
	a32 *= factor;
	a13 *= factor;
	a23 *= factor;
	a33 *= factor;

	return *this;
}


bool Matrix3f::isInvertible() const{return det() != 0.0f;}

float Matrix3f::cofactor(int row, int col) const 
{
	if (row == 1 && col == 1) return a22 * a33 - a23 * a32;
	if (row == 1 && col == 2) return -(a21 * a33 - a23 * a31);
	if (row == 1 && col == 3) return a21 * a32 - a22 * a31;
	if (row == 2 && col == 1) return -(a12 * a33 - a13 * a32);
	if (row == 2 && col == 2) return a11 * a33 - a13 * a31;
	if (row == 2 && col == 3) return -(a11 * a32 - a12 * a31);
	if (row == 3 && col == 1) return a12 * a23 - a13 * a22;
	if (row == 3 && col == 2) return -(a11 * a23 - a13 * a21);
	if (row == 3 && col == 3) return a11 * a22 - a12 * a21;
	return 0.0f;
}

Matrix3f Matrix3f::adjugate() const 
{
	return Matrix3f(
		cofactor(1, 1), cofactor(2, 1), cofactor(3, 1),
		cofactor(1, 2), cofactor(2, 2), cofactor(3, 2),
		cofactor(1, 3), cofactor(2, 3), cofactor(3, 3)
	);
}

Matrix3f Matrix3f::inverse() const 
{
	if (!isInvertible())
	{
		throw std::runtime_error("Matrix is not invertible.");
	}

	return adjugate() * (1.0f / det());		// faster than adjugate() / det()
}

Matrix3f& Matrix3f::transpose()
{
	float temp;
	temp = a12; a12 = a21; a21 = temp;
	temp = a13; a13 = a31; a31 = temp;
	temp = a23; a23 = a32; a32 = temp;
	return *this;
}

Matrix3f Matrix3f::getTranspose() const
{
	Matrix3f result;
	result.a11 = a11; result.a12 = a21; result.a13 = a31;
	result.a21 = a12; result.a22 = a22; result.a23 = a32;
	result.a31 = a13; result.a32 = a23; result.a33 = a33;
	return result;
}
