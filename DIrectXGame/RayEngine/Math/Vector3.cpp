#include "Vector3.h"
#include <cmath>

// 全ての値が0のベクトル
const Vector3 Vector3::zero = { 0.0f, 0.0f, 0.0f };
// 全ての値が1のベクトル
const Vector3 Vector3::one = { 1.0f, 1.0f, 1.0f };


float Vector3::Length() const {
	return sqrtf((*this) * (*this));
}

Vector3 Vector3::Normarize() const {
	float length = this->Length();
	if (length != 0) {
		return *this / length;
	}
	else {
		return { 0.0f, 0.0f, 0.0f };
	}
}

Vector3 Vector3::operator+(const Vector3& v2) const {
	Vector3 result;
	result.x = this->x + v2.x;
	result.y = this->y + v2.y;
	result.z = this->z + v2.z;
	return result;
}

Vector3 Vector3::operator-(const Vector3& v2) const {
	Vector3 result;
	result.x = this->x - v2.x;
	result.y = this->y - v2.y;
	result.z = this->z - v2.z;
	return result;
}

Vector3 Vector3::operator*(const float& value) const {
	Vector3 result;
	result.x = this->x * value;
	result.y = this->y * value;
	result.z = this->z * value;
	return result;
}

Vector3 Vector3::operator/(const float& value) const {
	Vector3 result;
	result.x = this->x / value;
	result.y = this->y / value;
	result.z = this->z / value;
	return result;
}

float Vector3::operator*(const Vector3& v2) const {
	float result;
	result = this->x * v2.x + this->y * v2.y + this->z * v2.z;
	return result;
}

Vector3 Vector3::cross(const Vector3& v2) const {
	Vector3 result;
	result.x = y * v2.z - z * v2.y;
	result.x = z * v2.x - x * v2.z;
	result.x = x * v2.y - y * v2.x;

	return result;
}

//Vector3 Vector3::operator%(const Vector3& v2) const {
//	Vector3 result;
//	result.x = this->x % v2.x;
//	result.y = this->y % v2.y;
//	result.z = z % v2.z;
//	return result;
//}
