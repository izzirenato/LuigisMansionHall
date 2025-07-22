#include "math/Matrix4f.h"
#include <iostream>
#include "math/Matrix3f.h"

//ctors
Matrix4f::Matrix4f()
{
	a11 = 1, a22 = 1, a33 = 1, a44 = 1;
	a12 = 0, a13 = 0, a14 = 0;
	a21 = 0, a23 = 0, a24 = 0;
	a31 = 0, a32 = 0, a34 = 0;
	a41 = 0, a42 = 0, a43 = 0;
}

Matrix4f::Matrix4f(Vector4f i, Vector4f j, Vector4f k, Vector4f l)
{
	a11 = i.x, a21 = i.y, a31 = i.z, a41 = i.w;
	a12 = j.x, a22 = j.y, a32 = j.z, a42 = j.w;
	a13 = k.x, a23 = k.y, a33 = k.z, a43 = k.w;
	a14 = l.x, a24 = l.y, a34 = l.z, a44 = l.w;
}

Matrix4f::Matrix4f(float value) : a11(value), a12(value), a13(value), a14(value),
a21(value), a22(value), a23(value), a24(value),
a31(value), a32(value), a33(value), a34(value),
a41(value), a42(value), a43(value), a44(value) {}

Matrix4f::Matrix4f(float a11, float a21, float a31, float a41,
	float a12, float a22, float a32, float a42,
	float a13, float a23, float a33, float a43,
	float a14, float a24, float a34, float a44) :
	a11(a11), a21(a21), a31(a31), a41(a41),
	a12(a12), a22(a22), a32(a32), a42(a42),
	a13(a13), a23(a23), a33(a33), a43(a43),
	a14(a14), a24(a24), a34(a34), a44(a44) {
}

Matrix4f::Matrix4f(Vector3f translation)
{
	a11 = 1, a21 = 0, a31 = 0, a41 = translation.x;
	a12 = 0, a22 = 1, a32 = 0, a42 = translation.y;
	a13 = 0, a23 = 0, a33 = 1, a43 = translation.z;
	a14 = 0, a24 = 0, a34 = 0, a44 = 1;
}


Matrix4f& Matrix4f::transpose()
{
	std::swap(a12, a21);
	std::swap(a13, a31);
	std::swap(a14, a41);
	std::swap(a23, a32);
	std::swap(a24, a42);
	std::swap(a34, a43);
	return *this;
}


// Overload +
Matrix4f Matrix4f::operator+(const Matrix4f& other) const
{
	return Matrix4f(a11 + other.a11, a21 + other.a21, a31 + other.a31, a41 + other.a41,
		a12 + other.a12, a22 + other.a22, a32 + other.a32, a42 + other.a42,
		a13 + other.a13, a23 + other.a23, a33 + other.a33, a43 + other.a43,
		a14 + other.a14, a24 + other.a24, a34 + other.a34, a44 + other.a44);
}

// Overload -
Matrix4f Matrix4f::operator-(const Matrix4f& other) const
{
	return Matrix4f(a11 - other.a11, a21 - other.a21, a31 - other.a31, a41 - other.a41,
		a12 - other.a12, a22 - other.a22, a32 - other.a32, a42 - other.a42,
		a13 - other.a13, a23 - other.a23, a33 - other.a33, a43 - other.a43,
		a14 - other.a14, a24 - other.a24, a34 - other.a34, a44 - other.a44);
}

// Overload * multiply with a scalar
Matrix4f Matrix4f::operator*(float scalar) const
{
	return Matrix4f(a11 * scalar, a21 * scalar, a31 * scalar, a41 * scalar,
		a12 * scalar, a22 * scalar, a32 * scalar, a42 * scalar,
		a13 * scalar, a23 * scalar, a33 * scalar, a43 * scalar,
		a14 * scalar, a24 * scalar, a34 * scalar, a44 * scalar);
}


// Overload * multiply with a Matrix
Matrix4f Matrix4f::operator* (const Matrix4f& other) const
{
	Vector4f r1 = Vector4f(a11, a12, a13, a14);
	Vector4f r2 = Vector4f(a21, a22, a23, a24);
	Vector4f r3 = Vector4f(a31, a32, a33, a34);
	Vector4f r4 = Vector4f(a41, a42, a43, a44);

	Vector4f c1 = Vector4f(other.a11, other.a21, other.a31, other.a41);
	Vector4f c2 = Vector4f(other.a12, other.a22, other.a32, other.a42);
	Vector4f c3 = Vector4f(other.a13, other.a23, other.a33, other.a43);
	Vector4f c4 = Vector4f(other.a14, other.a24, other.a34, other.a44);

	return Matrix4f(r1.dot(c1), r2.dot(c1), r3.dot(c1), r4.dot(c1),
		r1.dot(c2), r2.dot(c2), r3.dot(c2), r4.dot(c2),
		r1.dot(c3), r2.dot(c3), r3.dot(c3), r4.dot(c3),
		r1.dot(c4), r2.dot(c4), r3.dot(c4), r4.dot(c4));
}

// Overload * multiply with a vector
Vector4f Matrix4f::operator* (const Vector4f v) const
{
	Vector4f r1 = Vector4f(a11, a12, a13, a14);
	Vector4f r2 = Vector4f(a21, a22, a23, a24);
	Vector4f r3 = Vector4f(a31, a32, a33, a34);
	Vector4f r4 = Vector4f(a41, a42, a43, a44);

	return Vector4f(r1.dot(v), r2.dot(v), r3.dot(v), r4.dot(v));
}

// Overload / division with a scalar
Matrix4f Matrix4f::operator/(float scalar) const
{
	float factor = 1.0f / scalar;
	return Matrix4f(a11 * factor, a21 * factor, a31 * factor, a41 * factor,
		a12 * factor, a22 * factor, a32 * factor, a42 * factor,
		a13 * factor, a23 * factor, a33 * factor, a43 * factor,
		a14 * factor, a24 * factor, a34 * factor, a44 * factor);
}


// Overload +=
Matrix4f& Matrix4f::operator+=(const Matrix4f& other)
{
	a11 += other.a11; a21 += other.a21; a31 += other.a31; a41 += other.a41;
	a12 += other.a12; a22 += other.a22; a32 += other.a32; a42 += other.a42;
	a13 += other.a13; a23 += other.a23; a33 += other.a33; a43 += other.a43;
	a14 += other.a14; a24 += other.a24; a34 += other.a34; a44 += other.a44;
	return *this;
}

// Overload -=
Matrix4f& Matrix4f::operator-=(const Matrix4f& other)
{
	a11 -= other.a11; a21 -= other.a21; a31 -= other.a31; a41 -= other.a41;
	a12 -= other.a12; a22 -= other.a22; a32 -= other.a32; a42 -= other.a42;
	a13 -= other.a13; a23 -= other.a23; a33 -= other.a33; a43 -= other.a43;
	a14 -= other.a14; a24 -= other.a24; a34 -= other.a34; a44 -= other.a44;
	return *this;
}

// Overload *=
Matrix4f& Matrix4f::operator*=(float scalar)
{
	a11 *= scalar; a21 *= scalar; a31 *= scalar; a41 *= scalar;
	a12 *= scalar; a22 *= scalar; a32 *= scalar; a42 *= scalar;
	a13 *= scalar; a23 *= scalar; a33 *= scalar; a43 *= scalar;
	a14 *= scalar; a24 *= scalar; a34 *= scalar; a44 *= scalar;
	return *this;
}

Matrix4f& Matrix4f::operator*=(const Matrix4f& other)
{
	Matrix4f temp = *this;

	// Create row vectors from this matrix
	Vector4f r1(temp.a11, temp.a12, temp.a13, temp.a14);
	Vector4f r2(temp.a21, temp.a22, temp.a23, temp.a24);
	Vector4f r3(temp.a31, temp.a32, temp.a33, temp.a34);
	Vector4f r4(temp.a41, temp.a42, temp.a43, temp.a44);

	// Create column vectors from the other matrix
	Vector4f c1(other.a11, other.a21, other.a31, other.a41);
	Vector4f c2(other.a12, other.a22, other.a32, other.a42);
	Vector4f c3(other.a13, other.a23, other.a33, other.a43);
	Vector4f c4(other.a14, other.a24, other.a34, other.a44);

	// Compute the dot products for each position in the result matrix
	a11 = r1.dot(c1); a12 = r1.dot(c2); a13 = r1.dot(c3); a14 = r1.dot(c4);
	a21 = r2.dot(c1); a22 = r2.dot(c2); a23 = r2.dot(c3); a24 = r2.dot(c4);
	a31 = r3.dot(c1); a32 = r3.dot(c2); a33 = r3.dot(c3); a34 = r3.dot(c4);
	a41 = r4.dot(c1); a42 = r4.dot(c2); a43 = r4.dot(c3); a44 = r4.dot(c4);
	return *this;
}

// overload /
Matrix4f& Matrix4f::operator/=(float scalar)
{
	float factor = 1.0f / scalar;
	a11 *= factor; a21 *= factor; a31 *= factor; a41 *= factor;
	a12 *= factor; a22 *= factor; a32 *= factor; a42 *= factor;
	a13 *= factor; a23 *= factor; a33 *= factor; a43 *= factor;
	a14 *= factor; a24 *= factor; a34 *= factor; a44 *= factor;
	return *this;
}


bool Matrix4f::isInvertible() const {return det() != 0.0f;}

float Matrix4f::det() const
{
    try 
	{
        // Copy original matrix elements for LU decomposition
        float l11 = a11, l21 = a21, l31 = a31, l41 = a41;
        float l22 = a22, l32 = a32, l42 = a42;
        float l33 = a33, l43 = a43;
        float l44 = a44;

        // Check first pivot
        if (std::abs(l11) < 1e-7f) 
		{
            // If LU decomposition fails, fall back to Laplace expansion
            throw std::runtime_error("Zero pivot");
        }

        // Calculate U coefficients (upper triangular)
        float u12 = a12 / l11, u13 = a13 / l11, u14 = a14 / l11;

        // Update L coefficients (lower triangular)
        l21 -= l11 * u12;
        l31 -= l11 * u13;
        l41 -= l11 * u14;

        // Check second pivot
        if (std::abs(l22) < 1e-7f) 
		{
            throw std::runtime_error("Zero pivot");
        }

        // Continue LU decomposition
        float u23 = a23 / l22, u24 = a24 / l22;
        l32 -= l22 * u23;
        l42 -= l22 * u24;

        if (std::abs(l33) < 1e-7f) 
		{
            throw std::runtime_error("Zero pivot");
        }

        float u34 = a34 / l33;
        l43 -= l33 * u34;

        if (std::abs(l44) < 1e-7f) 
		{
            throw std::runtime_error("Zero pivot");
        }

        // Calculate determinant as product of diagonal elements
        return l11 * l22 * l33 * l44;
    }
    catch (const std::runtime_error&) 
	{
        // Fallback to Laplace expansion along first row
        return a11 * cofactor(0, 0) -
            a12 * cofactor(0, 1) +
            a13 * cofactor(0, 2) -
            a14 * cofactor(0, 3);
    }
}

// helper function to calculate the cofactor
float Matrix4f::cofactor(int row, int col) const
{
	float minor[9];
	int index = 0;

	for (int i = 0; i < 4; i++)
	{
		if (i == row) continue;

		for (int j = 0; j < 4; j++)
		{
			if (j == col) continue;

			float* elements[] = 
			{
				&((Matrix4f*)this)->a11, &((Matrix4f*)this)->a12, &((Matrix4f*)this)->a13, &((Matrix4f*)this)->a14,
				&((Matrix4f*)this)->a21, &((Matrix4f*)this)->a22, &((Matrix4f*)this)->a23, &((Matrix4f*)this)->a24,
				&((Matrix4f*)this)->a31, &((Matrix4f*)this)->a32, &((Matrix4f*)this)->a33, &((Matrix4f*)this)->a34,
				&((Matrix4f*)this)->a41, &((Matrix4f*)this)->a42, &((Matrix4f*)this)->a43, &((Matrix4f*)this)->a44
			};

			minor[index++] = *elements[i * 4 + j];
		}
	}

	float det =
		minor[0] * minor[4] * minor[8] +
		minor[1] * minor[5] * minor[6] +
		minor[2] * minor[3] * minor[7] -
		minor[2] * minor[4] * minor[6] -
		minor[0] * minor[5] * minor[7] -
		minor[1] * minor[3] * minor[8];

	float sign = ((row + col) % 2 == 0) ? 1.0f : -1.0f;

	return sign * det;



}


Matrix4f& Matrix4f::projection(float left, float right, float bottom, float top, float near, float far)
{
	float rl = right - left;
	float tb = top - bottom;
	float fn = far - near;

	a11 = (2.0f * near) / rl;
	a12 = 0.0f;
	a13 = 0.0f;
	a14 = 0.0f;

	a21 = 0.0f;
	a22 = (2.0f * near) / tb;
	a23 = 0.0f;
	a24 = 0.0f;

	a31 = (right + left) / rl;
	a32 = (top + bottom) / tb;
	a33 = -(far + near) / fn;
	a34 = -1.0f;

	a41 = 0.0f;
	a42 = 0.0f;
	a43 = -(2.0f * far * near) / fn;
	a44 = 0.0f;

	return *this;
}


Matrix4f& Matrix4f::lookAt(const Vector3f& eye, const Vector3f& lookAt, const Vector3f& up)
{
	Vector3f z = (eye-lookAt).normalize();     // forward
	Vector3f x = up.cross(z).normalize();        // right
	Vector3f y = z.cross(x).normalize();

	a11 = x.x;  a12 = x.y;  a13 = x.z;  a14 = -x.dot(eye);
	a21 = y.x;  a22 = y.y;  a23 = y.z;  a24 = -y.dot(eye);
	a31 = z.x;  a32 = z.y;  a33 = z.z;  a34 = -z.dot(eye);
	a41 = 0.0f; a42 = 0.0f; a43 = 0.0f; a44 = 1.0f;

	return *this;
}
