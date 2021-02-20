#pragma once
#include "Define.h"

class Vector3
{
public:
	float x;
	float y;
	float z;

	Vector3(float x, float y, float z) : x(x), y(y), z(z) {}

	Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

	void Set(float x, float y, float z)	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	friend Vector3 operator+(const Vector3& left, const Vector3& right) {
		return Vector3(left.x + right.x, left.y + right.y, left.z + right.z);
	}

	friend Vector3 operator-(const Vector3& left, const Vector3& right) {
		return Vector3(left.x - right.x, left.y - right.y, left.z - right.z);
	}

	// Component-wise multiplication
	friend Vector3 operator*(const Vector3& left, const Vector3& right) {
		return Vector3(left.x * right.x, left.y * right.y, left.z * right.z);
	}

	// Scalar multiply
	friend Vector3 operator*(float scalar, const Vector3& vector) {
		return Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	friend Vector3 operator*(const Vector3& vector, float scalar) {
		return Vector3(vector.x * scalar, vector.y * scalar, vector.z * scalar);
	}

	Vector3& operator*=(float inScalar)	{
		x *= inScalar;
		y *= inScalar;
		z *= inScalar;
		return *this;
	}

	Vector3& operator+=(const Vector3& inRight) {
		x += inRight.x;
		y += inRight.y;
		z += inRight.z;
		return *this;
	}

	Vector3& operator-=(const Vector3& inRight)	{
		x -= inRight.x;
		y -= inRight.y;
		z -= inRight.z;
		return *this;
	}

	float Length() {
		return sqrtf(x * x + y * y + z * z);
	}

	float LengthSq() {
		return x * x + y * y + z * z;
	}

	float Length2D() {
		return sqrtf(x * x + y * y);
	}

	float LengthSq2D() {
		return x * x + y * y;
	}

	void Normalize() {
		float length = Length();
		x /= length;
		y /= length;
		z /= length;
	}

	void Normalize2D() {
		float length = Length2D();
		x /= length;
		y /= length;
	}

	friend float Dot(const Vector3& inLeft, const Vector3& inRight)	{
		return (inLeft.x * inRight.x + inLeft.y * inRight.y + inLeft.z * inRight.z);
	}

	friend float Dot2D(const Vector3& inLeft, const Vector3& inRight) {
		return (inLeft.x * inRight.x + inLeft.y * inRight.y);
	}

	friend Vector3 Cross(const Vector3& inLeft, const Vector3& inRight)	{
		Vector3 temp;
		temp.x = inLeft.y * inRight.z - inLeft.z * inRight.y;
		temp.y = inLeft.z * inRight.x - inLeft.x * inRight.z;
		temp.z = inLeft.x * inRight.y - inLeft.y * inRight.x;
		return temp;
	}

	friend Vector3 Lerp(const Vector3& inA, const Vector3& inB, float t) {
		return Vector3(inA + t * (inB - inA));
	}

	static const Vector3 Zero;
	static const Vector3 UnitX;
	static const Vector3 UnitY;
	static const Vector3 UnitZ;
};


class Quaternion
{
public:
	float x, y, z, w;
};