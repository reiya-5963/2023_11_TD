#pragma once
struct Vector3 final {
	float x;	//!< x
	float y;	//!< y
	float z;	//!< z

	Vector3 operator+(const Vector3& v2) const;

	Vector3 operator-(const Vector3& v2) const;

	Vector3 operator*(const float& value) const;

	Vector3 operator/(const float& value) const;

	// ベクトルの大きさ
	float Length() const;

	// 正規化
	Vector3 Normarize() const;

	// 内積
	float operator*(const Vector3& v2) const;

	// 外積
	Vector3 cross(const Vector3& v2) const;

	static const Vector3 zero;
	static const Vector3 one;
};

