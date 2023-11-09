#include "MyMath.h"

#include <cassert>
#include <cmath>
#include <list>
#include <vector>
#include "PrimitiveDrawer.h"

#include "Vector3.h"
#include "Plane.h"

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{
	    v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
	    v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
	    v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};
	return result;
}

Matrix4x4 NotTransform(const Matrix4x4& matrix) {
	return {
		matrix.m[0][0],matrix.m[0][1] ,matrix.m[0][2] ,matrix.m[0][3],
		matrix.m[1][0],matrix.m[1][1] ,matrix.m[1][2] ,matrix.m[1][3],
		matrix.m[2][0],matrix.m[2][1] ,matrix.m[2][2] ,matrix.m[2][3],
		0.0f,0.0f,0.0f,matrix.m[3][3], };
}

Vector3 Lerp(const Vector3& start, const Vector3& end, float t) {
	return start + ((end - start) * t);
}

float Lerp(float start, float end, float t) { 
	return start + ((end - start) * t);
}


Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) {
	// 2つのベクトルの内積を計算
	const float cosTheta = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
	if (cosTheta < -1.0f || cosTheta > 1.0f) {
		return Lerp(v1,v2,t);
	}
	// 角度（ラジアン）を計算
	const float theta = std::acos(cosTheta);

	// sin(x) / sin(theta) を計算するために、thetaが0に近い場合は線形補間を行う
	if (std::abs(theta) < 0.0001f) {
		return v1 * (1.0f - t) + v2 * t;
	}

	// sin(theta) を計算
	const float sinTheta = std::sin(theta);

	// 0除算による計算不可を回避するため、sinThetaが0に近い場合は線形補間を行う
	if (std::abs(sinTheta) < 0.0001f) {
		return v1 * (1.0f - t) + v2 * t;
	}

	// パラメータによって補間するベクトルを計算
	const float w1 = std::sin((1.0f - t) * theta) / sinTheta;
	const float w2 = std::sin(t * theta) / sinTheta;
	const Vector3 result = v1 * w1 + v2 * w2;

	return result;
}

Vector3 CatmullRom(
    Vector3 Position0, Vector3 Position1, Vector3 Position2, Vector3 Position3, float t) {
	Vector3 Result;

	float t2 = t * t;
	float t3 = t2 * t;

	float P0 = -t3 + 2.0f * t2 - t;
	float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float P2 = -3.0f * t3 + 4.0f * t2 + t;
	float P3 = t3 - t2;

	Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;
	Result.z = (P0 * Position0.z + P1 * Position1.z + P2 * Position2.z + P3 * Position3.z) * 0.5f;

	return Result;
}

float Distance(const Vector3& v1, const Vector3& v2) {
	return static_cast<float>(
	    std::pow(v2.x - v1.x, 2) + std::pow(v2.y - v1.y, 2) + std::pow(v2.z - v1.z, 2));
}

float Dot(const Vector3& a, const Vector3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; }

float Outer(const Vector2& v1, const Vector2& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	return {(v1.x - v2.x), (v1.y - v2.y), (v1.z - v2.z)};
}

float Length(const Vector3& a) { return sqrtf((a.x * a.x) + (a.y * a.y) + (a.z * a.z)); }

Vector3 Normalize(const Vector3& v1){
	Vector3 result = v1;
	float length = result.Length();
	result.x /= length;
	result.y /= length;
	result.z /= length;
	return result;
}

Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 mat;
	mat.m[0][0] = m1.m[0][0] + m2.m[0][0], mat.m[0][1] = m1.m[0][1] + m2.m[0][1],
	mat.m[0][2] = m1.m[0][2] + m2.m[0][2], mat.m[0][3] = m1.m[0][3] + m2.m[0][3];
	mat.m[1][0] = m1.m[1][0] + m2.m[1][0], mat.m[1][1] = m1.m[1][1] + m2.m[1][1],
	mat.m[1][2] = m1.m[1][2] + m2.m[1][2], mat.m[1][3] = m1.m[1][3] + m2.m[1][3];
	mat.m[2][0] = m1.m[2][0] + m2.m[2][0], mat.m[2][1] = m1.m[2][1] + m2.m[2][1],
	mat.m[2][2] = m1.m[2][2] + m2.m[2][2], mat.m[2][3] = m1.m[2][3] + m2.m[2][3];
	mat.m[3][0] = m1.m[3][0] + m2.m[3][0], mat.m[3][1] = m1.m[3][1] + m2.m[3][1],
	mat.m[3][2] = m1.m[3][2] + m2.m[3][2], mat.m[3][3] = m1.m[3][3] + m2.m[3][3];
	return mat;
}

Matrix4x4 Sub(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 mat;
	mat.m[0][0] = m1.m[0][0] - m2.m[0][0], mat.m[0][1] = m1.m[0][1] - m2.m[0][1],
	mat.m[0][2] = m1.m[0][2] - m2.m[0][2], mat.m[0][3] = m1.m[0][3] - m2.m[0][3];
	mat.m[1][0] = m1.m[1][0] - m2.m[1][0], mat.m[1][1] = m1.m[1][1] - m2.m[1][1],
	mat.m[1][2] = m1.m[1][2] - m2.m[1][2], mat.m[1][3] = m1.m[1][3] - m2.m[1][3];
	mat.m[2][0] = m1.m[2][0] - m2.m[2][0], mat.m[2][1] = m1.m[2][1] - m2.m[2][1],
	mat.m[2][2] = m1.m[2][2] - m2.m[2][2], mat.m[2][3] = m1.m[2][3] - m2.m[2][3];
	mat.m[3][0] = m1.m[3][0] - m2.m[3][0], mat.m[3][1] = m1.m[3][1] - m2.m[3][1],
	mat.m[3][2] = m1.m[3][2] - m2.m[3][2], mat.m[3][3] = m1.m[3][3] - m2.m[3][3];
	return mat;
}

Matrix4x4 Mul(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 mat;
	mat.m[0][0] = m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] +
	              m1.m[0][3] * m2.m[3][0],
	mat.m[0][1] = m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] +
	              m1.m[0][3] * m2.m[3][1],
	mat.m[0][2] = m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] +
	              m1.m[0][3] * m2.m[3][2],
	mat.m[0][3] = m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] +
	              m1.m[0][3] * m2.m[3][3],

	mat.m[1][0] = m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] +
	              m1.m[1][3] * m2.m[3][0],
	mat.m[1][1] = m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] +
	              m1.m[1][3] * m2.m[3][1],
	mat.m[1][2] = m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] +
	              m1.m[1][3] * m2.m[3][2],
	mat.m[1][3] = m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] +
	              m1.m[1][3] * m2.m[3][3],

	mat.m[2][0] = m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] +
	              m1.m[2][3] * m2.m[3][0],
	mat.m[2][1] = m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] +
	              m1.m[2][3] * m2.m[3][1],
	mat.m[2][2] = m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] +
	              m1.m[2][3] * m2.m[3][2],
	mat.m[2][3] = m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] +
	              m1.m[2][3] * m2.m[3][3],

	mat.m[3][0] = m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] +
	              m1.m[3][3] * m2.m[3][0],
	mat.m[3][1] = m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] +
	              m1.m[3][3] * m2.m[3][1],
	mat.m[3][2] = m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] +
	              m1.m[3][3] * m2.m[3][2],
	mat.m[3][3] = m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] +
	              m1.m[3][3] * m2.m[3][3];

	return mat;
}

Matrix4x4 Mul(const float scaler, const Matrix4x4& m2) {
	Matrix4x4 mat;
	mat.m[0][0] = scaler * m2.m[0][0], mat.m[0][1] = scaler * m2.m[0][1],
	mat.m[0][2] = scaler * m2.m[0][2], mat.m[0][3] = scaler * m2.m[0][3];
	mat.m[1][0] = scaler * m2.m[1][0], mat.m[1][1] = scaler * m2.m[1][1],
	mat.m[1][2] = scaler * m2.m[1][2], mat.m[1][3] = scaler * m2.m[1][3];
	mat.m[2][0] = scaler * m2.m[2][0], mat.m[2][1] = scaler * m2.m[2][1],
	mat.m[2][2] = scaler * m2.m[2][2], mat.m[2][3] = scaler * m2.m[2][3];
	mat.m[3][0] = scaler * m2.m[3][0], mat.m[3][1] = scaler * m2.m[3][1],
	mat.m[3][2] = scaler * m2.m[3][2], mat.m[3][3] = scaler * m2.m[3][3];
	return mat;
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	float A;
	A = m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3] +
	    m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1] +
	    m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2] -
	    m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1] -
	    m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3] -
	    m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2] -
	    m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3] -
	    m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1] -
	    m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2] +
	    m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1] +
	    m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3] +
	    m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2] +
	    m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3] +
	    m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1] +
	    m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2] -
	    m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1] -
	    m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3] -
	    m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2] -
	    m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0] -
	    m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0] -
	    m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0] +
	    m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0] +
	    m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0] +
	    m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0];

	Matrix4x4 tmp;
	tmp.m[0][0] = 1.0f / A *
	              (m.m[1][1] * m.m[2][2] * m.m[3][3] + m.m[1][2] * m.m[2][3] * m.m[3][1] +
	               m.m[1][3] * m.m[2][1] * m.m[3][2] - m.m[1][3] * m.m[2][2] * m.m[3][1] -
	               m.m[1][2] * m.m[2][1] * m.m[3][3] - m.m[1][1] * m.m[2][3] * m.m[3][2]);

	tmp.m[0][1] = 1.0f / A *
	              (-m.m[0][1] * m.m[2][2] * m.m[3][3] - m.m[0][2] * m.m[2][3] * m.m[3][1] -
	               m.m[0][3] * m.m[2][1] * m.m[3][2] + m.m[0][3] * m.m[2][2] * m.m[3][1] +
	               m.m[0][2] * m.m[2][1] * m.m[3][3] + m.m[0][1] * m.m[2][3] * m.m[3][2]);

	tmp.m[0][2] = 1.0f / A *
	              (m.m[0][1] * m.m[1][2] * m.m[3][3] + m.m[0][2] * m.m[1][3] * m.m[3][1] +
	               m.m[0][3] * m.m[1][1] * m.m[3][2] - m.m[0][3] * m.m[1][2] * m.m[3][1] -
	               m.m[0][2] * m.m[1][1] * m.m[3][3] - m.m[0][1] * m.m[1][3] * m.m[3][2]);

	tmp.m[0][3] = 1.0f / A *
	              (-m.m[0][1] * m.m[1][2] * m.m[2][3] - m.m[0][2] * m.m[1][3] * m.m[2][1] -
	               m.m[0][3] * m.m[1][1] * m.m[2][2] + m.m[0][3] * m.m[1][2] * m.m[2][1] +
	               m.m[0][2] * m.m[1][1] * m.m[2][3] + m.m[0][1] * m.m[1][3] * m.m[2][2]);

	tmp.m[1][0] = 1.0f / A *
	              (-m.m[1][0] * m.m[2][2] * m.m[3][3] - m.m[1][2] * m.m[2][3] * m.m[3][0] -
	               m.m[1][3] * m.m[2][0] * m.m[3][2] + m.m[1][3] * m.m[2][2] * m.m[3][0] +
	               m.m[1][2] * m.m[2][0] * m.m[3][3] + m.m[1][0] * m.m[2][3] * m.m[3][2]);

	tmp.m[1][1] = 1.0f / A *
	              (m.m[0][0] * m.m[2][2] * m.m[3][3] + m.m[0][2] * m.m[2][3] * m.m[3][0] +
	               m.m[0][3] * m.m[2][0] * m.m[3][2] - m.m[0][3] * m.m[2][2] * m.m[3][0] -
	               m.m[0][2] * m.m[2][0] * m.m[3][3] - m.m[0][0] * m.m[2][3] * m.m[3][2]);

	tmp.m[1][2] = 1.0f / A *
	              (-m.m[0][0] * m.m[1][2] * m.m[3][3] - m.m[0][2] * m.m[1][3] * m.m[3][0] -
	               m.m[0][3] * m.m[1][0] * m.m[3][2] + m.m[0][3] * m.m[1][2] * m.m[3][0] +
	               m.m[0][2] * m.m[1][0] * m.m[3][3] + m.m[0][0] * m.m[1][3] * m.m[3][2]);

	tmp.m[1][3] = 1.0f / A *
	              (m.m[0][0] * m.m[1][2] * m.m[2][3] + m.m[0][2] * m.m[1][3] * m.m[2][0] +
	               m.m[0][3] * m.m[1][0] * m.m[2][2] - m.m[0][3] * m.m[1][2] * m.m[2][0] -
	               m.m[0][2] * m.m[1][0] * m.m[2][3] - m.m[0][0] * m.m[1][3] * m.m[2][2]);

	tmp.m[2][0] = 1.0f / A *
	              (m.m[1][0] * m.m[2][1] * m.m[3][3] + m.m[1][1] * m.m[2][3] * m.m[3][0] +
	               m.m[1][3] * m.m[2][0] * m.m[3][1] - m.m[1][3] * m.m[2][1] * m.m[3][0] -
	               m.m[1][1] * m.m[2][0] * m.m[3][3] - m.m[1][0] * m.m[2][3] * m.m[3][1]);

	tmp.m[2][1] = 1.0f / A *
	              (-m.m[0][0] * m.m[2][1] * m.m[3][3] - m.m[0][1] * m.m[2][3] * m.m[3][0] -
	               m.m[0][3] * m.m[2][0] * m.m[3][1] + m.m[0][3] * m.m[2][1] * m.m[3][0] +
	               m.m[0][1] * m.m[2][0] * m.m[3][3] + m.m[0][0] * m.m[2][3] * m.m[3][1]);

	tmp.m[2][2] = 1.0f / A *
	              (m.m[0][0] * m.m[1][1] * m.m[3][3] + m.m[0][1] * m.m[1][3] * m.m[3][0] +
	               m.m[0][3] * m.m[1][0] * m.m[3][1] - m.m[0][3] * m.m[1][1] * m.m[3][0] -
	               m.m[0][1] * m.m[1][0] * m.m[3][3] - m.m[0][0] * m.m[1][3] * m.m[3][1]);

	tmp.m[2][3] = 1.0f / A *
	              (-m.m[0][0] * m.m[1][1] * m.m[2][3] - m.m[0][1] * m.m[1][3] * m.m[2][0] -
	               m.m[0][3] * m.m[1][0] * m.m[2][1] + m.m[0][3] * m.m[1][1] * m.m[2][0] +
	               m.m[0][1] * m.m[1][0] * m.m[2][3] + m.m[0][0] * m.m[1][3] * m.m[2][1]);

	tmp.m[3][0] = 1.0f / A *
	              (-m.m[1][0] * m.m[2][1] * m.m[3][2] - m.m[1][1] * m.m[2][2] * m.m[3][0] -
	               m.m[1][2] * m.m[2][0] * m.m[3][1] + m.m[1][2] * m.m[2][1] * m.m[3][0] +
	               m.m[1][1] * m.m[2][0] * m.m[3][2] + m.m[1][0] * m.m[2][2] * m.m[3][1]);

	tmp.m[3][1] = 1.0f / A *
	              (m.m[0][0] * m.m[2][1] * m.m[3][2] + m.m[0][1] * m.m[2][2] * m.m[3][0] +
	               m.m[0][2] * m.m[2][0] * m.m[3][1] - m.m[0][2] * m.m[2][1] * m.m[3][0] -
	               m.m[0][1] * m.m[2][0] * m.m[3][2] - m.m[0][0] * m.m[2][2] * m.m[3][1]);

	tmp.m[3][2] = 1.0f / A *
	              (-m.m[0][0] * m.m[1][1] * m.m[3][2] - m.m[0][1] * m.m[1][2] * m.m[3][0] -
	               m.m[0][2] * m.m[1][0] * m.m[3][1] + m.m[0][2] * m.m[1][1] * m.m[3][0] +
	               m.m[0][1] * m.m[1][0] * m.m[3][2] + m.m[0][0] * m.m[1][2] * m.m[3][1]);

	tmp.m[3][3] = 1.0f / A *
	              (m.m[0][0] * m.m[1][1] * m.m[2][2] + m.m[0][1] * m.m[1][2] * m.m[2][0] +
	               m.m[0][2] * m.m[1][0] * m.m[2][1] - m.m[0][2] * m.m[1][1] * m.m[2][0] -
	               m.m[0][1] * m.m[1][0] * m.m[2][2] - m.m[0][0] * m.m[1][2] * m.m[2][1]);

	return tmp;
}

Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 mat;
	mat.m[0][0] = m.m[0][0], mat.m[0][1] = m.m[1][0], mat.m[0][2] = m.m[2][0],
	mat.m[0][3] = m.m[3][0];
	mat.m[1][0] = m.m[0][1], mat.m[1][1] = m.m[1][1], mat.m[1][2] = m.m[2][1],
	mat.m[1][3] = m.m[3][1];
	mat.m[2][0] = m.m[0][2], mat.m[2][1] = m.m[1][2], mat.m[2][2] = m.m[2][2],
	mat.m[2][3] = m.m[3][2];
	mat.m[3][0] = m.m[0][3], mat.m[3][1] = m.m[1][3], mat.m[3][2] = m.m[2][3],
	mat.m[3][3] = m.m[3][3];
	return mat;
}

Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 mat;
	mat.m[0][0] = 1.0f, mat.m[0][1] = 0.0f, mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = 1.0f, mat.m[1][2] = 0.0f, mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = 1.0f, mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f, mat.m[3][2] = 0.0f, mat.m[3][3] = 1.0f;
	return mat;
}

Vector3 MakeTranslate(const Matrix4x4& mat) {
	return Vector3(mat.m[3][0], mat.m[3][1], mat.m[3][2]);
}

Matrix4x4 MakeTranslateMatrix(const Vector3& translate) {
	Matrix4x4 tmp;
	tmp.m[0][0] = 1.0f, tmp.m[0][1] = 0.0f, tmp.m[0][2] = 0.0f, tmp.m[0][3] = 0.0f;
	tmp.m[1][0] = 0.0f, tmp.m[1][1] = 1.0f, tmp.m[1][2] = 0.0f, tmp.m[1][3] = 0.0f;
	tmp.m[2][0] = 0.0f, tmp.m[2][1] = 0.0f, tmp.m[2][2] = 1.0f, tmp.m[2][3] = 0.0f;
	tmp.m[3][0] = translate.x, tmp.m[3][1] = translate.y, tmp.m[3][2] = translate.z,
	tmp.m[3][3] = 1.0f;
	return tmp;
}

Matrix4x4 MakeScaleMatrix(const Vector3& scale) {
	Matrix4x4 tmp;
	tmp.m[0][0] = scale.x, tmp.m[0][1] = 0.0f, tmp.m[0][2] = 0.0f, tmp.m[0][3] = 0.0f;
	tmp.m[1][0] = 0.0f, tmp.m[1][1] = scale.y, tmp.m[1][2] = 0.0f, tmp.m[1][3] = 0.0f;
	tmp.m[2][0] = 0.0f, tmp.m[2][1] = 0.0f, tmp.m[2][2] = scale.z, tmp.m[2][3] = 0.0f;
	tmp.m[3][0] = 0.0f, tmp.m[3][1] = 0.0f, tmp.m[3][2] = 0.0f, tmp.m[3][3] = 1.0f;
	return tmp;
}

Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
	Vector3 result{0.0f, 0.0f, 0.0f};
	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] +
	           1.0f * matrix.m[3][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] +
	           1.0f * matrix.m[3][1];
	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] +
	           1.0f * matrix.m[3][2];
	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] +
	          1.0f * matrix.m[3][3];

	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;
	result.z /= w;
	return result;
}

Matrix4x4 MakeRotateXMatrix(float radian) {
	Matrix4x4 mat;
	mat.m[0][0] = 1.0f, mat.m[0][1] = 0.0f, mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = std::cos(radian), mat.m[1][2] = std::sin(radian),
	mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = -std::sin(radian), mat.m[2][2] = std::cos(radian),
	mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f, mat.m[3][2] = 0.0f, mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeRotateYMatrix(float radian) {
	Matrix4x4 mat;
	mat.m[0][0] = std::cos(radian), mat.m[0][1] = 0.0f, mat.m[0][2] = -std::sin(radian),
	mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = 1.0f, mat.m[1][2] = 0.0f, mat.m[1][3] = 0.0f;
	mat.m[2][0] = std::sin(radian), mat.m[2][1] = 0.0f, mat.m[2][2] = std::cos(radian),
	mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f, mat.m[3][2] = 0.0f, mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeRotateZMatrix(float radian) {
	Matrix4x4 mat;
	mat.m[0][0] = std::cos(radian), mat.m[0][1] = std::sin(radian), mat.m[0][2] = 0.0f,
	mat.m[0][3] = 0.0f;
	mat.m[1][0] = -std::sin(radian), mat.m[1][1] = std::cos(radian), mat.m[1][2] = 0.0f,
	mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = 1.0f, mat.m[2][3] = 0.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f, mat.m[3][2] = 0.0f, mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeRotateXYZMatrix(const Vector3& rotation) {
	Matrix4x4 mat = MakeIdentity4x4();
	mat *= MakeRotateXMatrix(rotation.x);
	mat *= MakeRotateYMatrix(rotation.y);
	mat *= MakeRotateZMatrix(rotation.z);
	return mat;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	Matrix4x4 tmp = Mul(
	    MakeRotateXMatrix(rotate.x), Mul(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

	Matrix4x4 mat;
	mat.m[0][0] = scale.x * tmp.m[0][0], mat.m[0][1] = scale.x * tmp.m[0][1],
	mat.m[0][2] = scale.x * tmp.m[0][2], mat.m[0][3] = 0.0f;
	mat.m[1][0] = scale.y * tmp.m[1][0], mat.m[1][1] = scale.y * tmp.m[1][1],
	mat.m[1][2] = scale.y * tmp.m[1][2], mat.m[1][3] = 0.0f;
	mat.m[2][0] = scale.z * tmp.m[2][0], mat.m[2][1] = scale.z * tmp.m[2][1],
	mat.m[2][2] = scale.z * tmp.m[2][2], mat.m[2][3] = 0.0f;
	mat.m[3][0] = translate.x, mat.m[3][1] = translate.y, mat.m[3][2] = translate.z,
	mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 mat;
	mat.m[0][0] = (1.0f / aspectRatio) * (1.0f / std::tan(fovY / 2.0f)), mat.m[0][1] = 0.0f,
	mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = 1.0f / std::tan(fovY / 2.0f), mat.m[1][2] = 0.0f,
	mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = farClip / (farClip - nearClip),
	mat.m[2][3] = 1.0f;
	mat.m[3][0] = 0.0f, mat.m[3][1] = 0.0f,
	mat.m[3][2] = (-nearClip * farClip) / (farClip - nearClip), mat.m[3][3] = 0.0f;
	return mat;
}

Matrix4x4 MakeOrthographicMatrix(
    float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 mat;
	mat.m[0][0] = 2.0f / (right - left), mat.m[0][1] = 0.0f, mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = 2.0f / (top - bottom), mat.m[1][2] = 0.0f, mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = 1.0f / (farClip - nearClip),
	mat.m[2][3] = 0.0f;
	mat.m[3][0] = (left + right) / (left - right), mat.m[3][1] = (top + bottom) / (bottom - top),
	mat.m[3][2] = nearClip / (nearClip - farClip), mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeViewportMatrix(
    float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 mat;
	mat.m[0][0] = width / 2.0f, mat.m[0][1] = 0.0f, mat.m[0][2] = 0.0f, mat.m[0][3] = 0.0f;
	mat.m[1][0] = 0.0f, mat.m[1][1] = -height / 2.0f, mat.m[1][2] = 0.0f, mat.m[1][3] = 0.0f;
	mat.m[2][0] = 0.0f, mat.m[2][1] = 0.0f, mat.m[2][2] = maxDepth - minDepth, mat.m[2][3] = 0.0f;
	mat.m[3][0] = left + (width / 2.0f), mat.m[3][1] = top + (height / 2.0f),
	mat.m[3][2] = minDepth, mat.m[3][3] = 1.0f;
	return mat;
}

Matrix4x4 MakeViewMatrix(const Vector3& rotation, const Vector3& translation) {
	Matrix4x4 cameraMatrix = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), rotation, translation);
	return Inverse(cameraMatrix);
}

Matrix4x4 MakeViewProjectMatrixMatrix(const ViewProjection& viewProjection) {
	Matrix4x4 viewMatrix = Inverse(MakeAffineMatrix(
	    Vector3(1.0f, 1.0f, 1.0f), viewProjection.rotation_, viewProjection.translate_));
	return Mul(viewMatrix, viewProjection.matProjection_);
}

Matrix4x4 MakeLookAtLH(const Vector3& target, const Vector3& eye, const Vector3& up) {
	Vector3 zaxis = Normalize(target - eye);
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	Vector3 yaxis = Cross(zaxis, xaxis);
	return {
		xaxis.x,yaxis.x,zaxis.x,0.0f,
		xaxis.y,yaxis.y,zaxis.y,0.0f,
		xaxis.z,yaxis.z,zaxis.z,0.0f,
		-Dot(xaxis, eye),-Dot(yaxis, eye),-Dot(zaxis, eye),1.0f
	};
}

Matrix4x4 MakeBillboard(const Vector3& target, const Vector3& eye, const Vector3& up) {
	// ビルボード回転行列
	// X軸
	Vector3 zaxis = Normalize(target - eye);
	// Y軸
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	// Z軸
	Vector3 yaxis = Cross(zaxis, xaxis);
	return {
		xaxis.x,xaxis.y,xaxis.z,0.0f,
		yaxis.x,yaxis.y,yaxis.z,0.0f,
		zaxis.x,zaxis.y,zaxis.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
}

Matrix4x4 MakeBillboardYAxsizLook(const Vector3& target, const Vector3& eye, const Vector3& up) {
	// ビルボード回転行列
	// X軸
	Vector3 zaxis = Normalize(target - eye);
	// Y軸
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	// Z軸
	Vector3 yaxis = Cross(zaxis, xaxis);
	return {
		xaxis.x,xaxis.y,xaxis.z,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		zaxis.x,zaxis.y,zaxis.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
}

Matrix4x4 MakeBillboardXAxsizLook(const Vector3& target, const Vector3& eye, const Vector3& up) {
	// ビルボード回転行列
	// X軸
	Vector3 zaxis = Normalize(target - eye);
	// Y軸
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	// Z軸
	Vector3 yaxis = Cross(zaxis, xaxis);
	return {
		0.0f,0.0f,0.0f,0.0f,
		yaxis.x,yaxis.y,yaxis.z,0.0f,
		zaxis.x,zaxis.y,zaxis.z,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
}

Matrix4x4 MakeBillboardZAxsizLook(const Vector3& target, const Vector3& eye, const Vector3& up) {
	// ビルボード回転行列
	// X軸
	Vector3 zaxis = Normalize(target - eye);
	// Y軸
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	// Z軸
	Vector3 yaxis = Cross(zaxis, xaxis);
	return {
		xaxis.x,xaxis.y,xaxis.z,0.0f,
		yaxis.x,yaxis.y,yaxis.z,0.0f,
		0.0f,0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	};
}


Vector3 GetXAxis(const Matrix4x4& mat) {
	return Vector3(mat.m[0][0], mat.m[0][1], mat.m[0][2]);
}

Vector3 GetYAxis(const Matrix4x4& mat) {
	return Vector3(mat.m[1][0], mat.m[1][1], mat.m[1][2]);
}

Vector3 GetZAxis(const Matrix4x4& mat) {
	return Vector3(mat.m[2][0], mat.m[2][1], mat.m[2][2]);
}


Matrix4x4 Convert(const Matrix4x4& m1) {
	Matrix4x4 mat;
	mat.m[0][0] = m1.m[0][0], mat.m[0][1] = m1.m[0][1], mat.m[0][2] = m1.m[0][2],
	mat.m[0][3] = m1.m[0][3];
	mat.m[1][0] = m1.m[1][0], mat.m[1][1] = m1.m[1][1], mat.m[1][2] = m1.m[1][2],
	mat.m[1][3] = m1.m[1][3];
	mat.m[2][0] = m1.m[2][0], mat.m[2][1] = m1.m[2][1], mat.m[2][2] = m1.m[2][2],
	mat.m[2][3] = m1.m[2][3];
	mat.m[3][0] = m1.m[3][0], mat.m[3][1] = m1.m[3][1], mat.m[3][2] = m1.m[3][2],
	mat.m[3][3] = m1.m[3][3];
	return mat;
}

float Clamp(float num, float min, float max) {
	if (num <= min) {
		num = min;
	} else if (num >= max) {
		num = max;
	}
	return num;
}

float RadToDeg(float radian) { return static_cast<float>(PI) / radian; }

float DegToRad(float degree) { return degree * static_cast<float>(PI) / 180.0f; }

Matrix4x4 MakeMatWolrd(const WorldTransform& worldtransform) { 
	Matrix4x4 result = MakeIdentity4x4();
	result = MakeAffineMatrix(
		worldtransform.scale_, 
		worldtransform.rotation_, 
		worldtransform.translate_
	); 
	return result;
}

void ChackHitBox(const WorldTransform& worldtransform, const ViewProjection& viewProjection,const Vector4& color) {
	WorldTransform tmpWorldTransform = worldtransform;
	Vector3 vertices[] = {
	    {-tmpWorldTransform.scale_},
	    {tmpWorldTransform.scale_.x, -tmpWorldTransform.scale_.y, -tmpWorldTransform.scale_.z},
	    {tmpWorldTransform.scale_.x, -tmpWorldTransform.scale_.y, tmpWorldTransform.scale_.z},
	    {-tmpWorldTransform.scale_.x, tmpWorldTransform.scale_.y, tmpWorldTransform.scale_.z},
	    {-tmpWorldTransform.scale_.x, tmpWorldTransform.scale_.y, -tmpWorldTransform.scale_.z},
	    {tmpWorldTransform.scale_.x, tmpWorldTransform.scale_.y, -tmpWorldTransform.scale_.z},
	    {tmpWorldTransform.scale_},
	    {-tmpWorldTransform.scale_.x, tmpWorldTransform.scale_.y, tmpWorldTransform.scale_.z},
	};
	PrimitiveDrawer* line = PrimitiveDrawer::GetInstance();
	line->SetViewProjection(&viewProjection);
	Matrix4x4 viewMatrix = MakeViewProjectMatrixMatrix(viewProjection);
	for (int i = 0; i < 4; i++) {
		int j = (i + 1) % 4;
		line->SetDraw(
		    Transform(vertices[i], viewMatrix), 
			Transform(vertices[j], viewMatrix), 
			color);
		line->SetDraw(
		    Transform(vertices[i], viewMatrix), 
			Transform(vertices[i + 4], viewMatrix), 
			color);
		line->SetDraw(
		    Transform(vertices[i + 4], viewMatrix),
			Transform(vertices[j + 4], viewMatrix), 
			color);
	}

}

float LenpShortAngle(float a, float b, float t) { 
	 // 角度差分を求める
	float diff = b - a;
	// 角度[-2PI,+2PI]に補正
	diff = std::fmod(diff, DegToRad(360.0f));
	// 角度[-PI,+PI]に補正
	if (diff > DegToRad(180.0f)) {
		diff -= DegToRad(360.0f);
	} else if (diff < -DegToRad(180.0f)) {
		diff += DegToRad(360.0f);
	}
	return Lerp(a, a + diff, t);
}

Vector3 LenpShortAngle(const Vector3& a, const Vector3& b, float t) { 
	// 各軸の角度差分を求める
	float diffX = b.x - a.x;
	float diffY = b.y - a.y;
	float diffZ = b.z - a.z;

	// 各軸の角度[-2PI,+2PI]に補正
	diffX = std::fmod(diffX, DegToRad(360.0f));
	diffY = std::fmod(diffY, DegToRad(360.0f));
	diffZ = std::fmod(diffZ, DegToRad(360.0f));

	// 各軸の角度[-PI,+PI]に補正
	if (diffX > DegToRad(180.0f)) {
		diffX -= DegToRad(360.0f);
	} else if (diffX < -DegToRad(180.0f)) {
		diffX += DegToRad(360.0f);
	}

	if (diffY > DegToRad(180.0f)) {
		diffY -= DegToRad(360.0f);
	} else if (diffY < -DegToRad(180.0f)) {
		diffY += DegToRad(360.0f);
	}

	if (diffZ > DegToRad(180.0f)) {
		diffZ -= DegToRad(360.0f);
	} else if (diffZ < -DegToRad(180.0f)) {
		diffZ += DegToRad(360.0f);
	}

	// 補正された角度をLerpして返す
	return Vector3(Lerp(a.x, a.x + diffX, t), Lerp(a.y, a.y + diffY, t), Lerp(a.z, a.z + diffZ, t));
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return {-vector.y, vector.x, 0.0f};
	}
	return {0.0f, -vector.z, vector.y};
}

Vector3 Cross(const Vector3& a, const Vector3& b) {
	return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

AABB AABBAssignment(const AABB& aabb) {
	AABB result = aabb;
	// x
	result.min_.x = (std::min)(result.min_.x, result.max_.x);
	result.max_.x = (std::max)(result.min_.x, result.max_.x);
	// y
	result.min_.y = (std::min)(result.min_.y, result.max_.y);
	result.max_.y = (std::max)(result.min_.y, result.max_.y);
	// z
	result.min_.z = (std::min)(result.min_.z, result.max_.z);
	result.max_.z = (std::max)(result.min_.z, result.max_.z);

	return result;
}

OBB OBBSetRotate(const OBB& Obb, const Vector3& rotate) {// 回転行列を生成
	Matrix4x4 rotateMatrix = Mul(
	    MakeRotateXMatrix(rotate.x), Mul(MakeRotateYMatrix(rotate.y), MakeRotateZMatrix(rotate.z)));

	// 回転行列から軸を抽出
	OBB obb = Obb;

	obb.orientations_[0].x = rotateMatrix.m[0][0];
	obb.orientations_[0].y = rotateMatrix.m[0][1];
	obb.orientations_[0].z = rotateMatrix.m[0][2];

	obb.orientations_[1].x = rotateMatrix.m[1][0];
	obb.orientations_[1].y = rotateMatrix.m[1][1];
	obb.orientations_[1].z = rotateMatrix.m[1][2];

	obb.orientations_[2].x = rotateMatrix.m[2][0];
	obb.orientations_[2].y = rotateMatrix.m[2][1];
	obb.orientations_[2].z = rotateMatrix.m[2][2];

	return obb;
}
OBB OBBSetRotate(const OBB& Obb, const Vector3& rotate, const Vector3& rotate1) { // 回転行列を生成
	Matrix4x4 rotateMatrix = MakeRotateXMatrix(rotate.x) * MakeRotateXMatrix(rotate1.x) *
	                         MakeRotateYMatrix(rotate.y) * MakeRotateYMatrix(rotate1.y) *
	                         MakeRotateZMatrix(rotate.z) * MakeRotateZMatrix(rotate1.z);

	// 回転行列から軸を抽出
	OBB obb = Obb;

	obb.orientations_[0].x = rotateMatrix.m[0][0];
	obb.orientations_[0].y = rotateMatrix.m[0][1];
	obb.orientations_[0].z = rotateMatrix.m[0][2];

	obb.orientations_[1].x = rotateMatrix.m[1][0];
	obb.orientations_[1].y = rotateMatrix.m[1][1];
	obb.orientations_[1].z = rotateMatrix.m[1][2];

	obb.orientations_[2].x = rotateMatrix.m[2][0];
	obb.orientations_[2].y = rotateMatrix.m[2][1];
	obb.orientations_[2].z = rotateMatrix.m[2][2];

	return obb;
}

Matrix4x4 OBBMakeWorldMatrix(const OBB& obb) {
	Matrix4x4 result;
	result.m[0][0] = obb.orientations_[0].x;
	result.m[0][1] = obb.orientations_[0].y;
	result.m[0][2] = obb.orientations_[0].z;

	result.m[1][0] = obb.orientations_[1].x;
	result.m[1][1] = obb.orientations_[1].y;
	result.m[1][2] = obb.orientations_[1].z;

	result.m[2][0] = obb.orientations_[2].x;
	result.m[2][1] = obb.orientations_[2].y;
	result.m[2][2] = obb.orientations_[2].z;

	result.m[3][0] = obb.center_.x;
	result.m[3][1] = obb.center_.y;
	result.m[3][2] = obb.center_.z;
	return result;
}

Matrix4x4 SetRotate(const Vector3 (&array)[3]) {
	Matrix4x4 rotateMatrix;
	rotateMatrix = MakeIdentity4x4();
	rotateMatrix.m[0][0] = array[0].x;
	rotateMatrix.m[0][1] = array[0].y;
	rotateMatrix.m[0][2] = array[0].z;
	rotateMatrix.m[1][0] = array[1].x;
	rotateMatrix.m[1][1] = array[1].y;
	rotateMatrix.m[1][2] = array[1].z;
	rotateMatrix.m[2][0] = array[2].x;
	rotateMatrix.m[2][1] = array[2].y;
	rotateMatrix.m[2][2] = array[2].z;
	return rotateMatrix;
}

Vector2 QuadraticBezier(const Vector2& controlPoint0, const Vector2& controlPoint1, const Vector2& controlPoint2,float t) {
	// 制御点p0,p1を線形補間
	Vector2 p0p1 = Lerp(controlPoint0, controlPoint1, t);
	// 制御点p1,p2線形補間
	Vector2 p1p2 = Lerp(controlPoint1, controlPoint2, t);
	// 補間点p0p1,p1p2をさらに線形補間
	return Lerp(p0p1, p1p2, t);
}

Vector3 CubicBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,float t) {
	// 制御点p0,p1を線形補間
	Vector3 p0p1 = Lerp(controlPoint0, controlPoint1, t);
	// 制御点p1,p2線形補間
	Vector3 p1p2 = Lerp(controlPoint1, controlPoint2, t);
	// 補間点p0p1,p1p2をさらに線形補間
	return Lerp(p0p1, p1p2, t);
}

Vector2 QuadraticCatmullRom(const Vector2& Position0, const Vector2& Position1, const Vector2& Position2,const Vector2& Position3, float t) {
	Vector2 Result;

	float t2 = t * t;
	float t3 = t2 * t;

	float P0 = -t3 + 2.0f * t2 - t;
	float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float P2 = -3.0f * t3 + 4.0f * t2 + t;
	float P3 = t3 - t2;

	Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;

	return Result;
}

Vector3 CubicCatmullRom(const Vector3& Position0, const Vector3& Position1, const Vector3& Position2,const Vector3& Position3, float t) {
	Vector3 Result;

	float t2 = t * t;
	float t3 = t2 * t;

	float P0 = -t3 + 2.0f * t2 - t;
	float P1 = 3.0f * t3 - 5.0f * t2 + 2.0f;
	float P2 = -3.0f * t3 + 4.0f * t2 + t;
	float P3 = t3 - t2;

	Result.x = (P0 * Position0.x + P1 * Position1.x + P2 * Position2.x + P3 * Position3.x) * 0.5f;
	Result.y = (P0 * Position0.y + P1 * Position1.y + P2 * Position2.y + P3 * Position3.y) * 0.5f;
	Result.z = (P0 * Position0.z + P1 * Position1.z + P2 * Position2.z + P3 * Position3.z) * 0.5f;

	return Result;
}

void OBBIndex(const OBB& obb, std::vector<Vector3>& output_vertices) {
	std::vector<Vector3> vertices = {
	    {-obb.size_},
	    {+obb.size_.x, -obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, -obb.size_.y, +obb.size_.z},
	    {-obb.size_.x, -obb.size_.y, +obb.size_.z},
	    {-obb.size_.x, +obb.size_.y, -obb.size_.z},
	    {+obb.size_.x, +obb.size_.y, -obb.size_.z},
	    {obb.size_},
	    {-obb.size_.x, +obb.size_.y, +obb.size_.z},
	};

	Matrix4x4 rotateMatrix = SetRotate(obb.orientations_);
	for (auto& vertex : vertices) {
		vertex = Transform(vertex, rotateMatrix);
		vertex = vertex + obb.center_;
	}
	output_vertices = vertices;
}

bool SeparationAxis(const Vector3 axis, const OBB obb_1, const OBB obb_2) {// 分離軸
	Vector3 L = axis;
	// 頂点数
	const int32_t kIndex = 8;
	// 頂点格納用配列
	std::vector<Vector3> vertices_1;
	std::vector<Vector3> vertices_2;
	// 配列に頂点を代入
	OBBIndex(obb_1, vertices_1);
	OBBIndex(obb_2, vertices_2);
	// 距離を格納
	float min_1 = Dot(vertices_1[0], L);
	float max_1 = min_1;
	float min_2 = Dot(vertices_2[0], L);
	float max_2 = min_2;
	for (size_t i = 1; i < kIndex; i++) {
		float dot_1 = Dot(vertices_1[i], L);
		float dot_2 = Dot(vertices_2[i], L);
		// min/max比べる
		min_1 = (std::min)(min_1, dot_1);
		max_1 = (std::max)(max_1, dot_1);
		min_2 = (std::min)(min_2, dot_2);
		max_2 = (std::max)(max_2, dot_2);
	}
	float L1 = max_1 - min_1;
	float L2 = max_2 - min_2;
	float sumSpan = L1 + L2;
	float longSpan = (std::max)(max_1, max_2) - (std::min)(min_1, min_2);
	// 分離軸である
	if (sumSpan < longSpan) {
		return true;
	}
	return false;
}

bool IsInsideFrustum(const Sphere& sphere, const ViewProjection& viewProjection) {
	// カメラの視錐台を計算
	ViewProjection view = viewProjection;
	Matrix4x4 viewProjectionMatrix = view.matView_ * view.matProjection_;

	// カメラの視錐台の平面を計算
	Plane planes[6]{};
	for (int i = 0; i < 6; i++) {
		// カメラの視錐台の平面の法線ベクトルと距離を計算
		switch (i) {
		case 0: // 左平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][0];
			planes[i].normal_.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][0];
			planes[i].normal_.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][0];
			planes[i].distance_ = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][0];
			break;
		case 1: // 右平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][0];
			planes[i].normal_.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][0];
			planes[i].normal_.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][0];
			planes[i].distance_ = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][0];
			break;
		case 2: // 上平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][1];
			planes[i].normal_.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][1];
			planes[i].normal_.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][1];
			planes[i].distance_ = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][1];
			break;
		case 3: // 下平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][3] + viewProjectionMatrix.m[0][1];
			planes[i].normal_.y = viewProjectionMatrix.m[1][3] + viewProjectionMatrix.m[1][1];
			planes[i].normal_.z = viewProjectionMatrix.m[2][3] + viewProjectionMatrix.m[2][1];
			planes[i].distance_ = viewProjectionMatrix.m[3][3] + viewProjectionMatrix.m[3][1];
			break;
		case 4: // 近平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][2];
			planes[i].normal_.y = viewProjectionMatrix.m[1][2];
			planes[i].normal_.z = viewProjectionMatrix.m[2][2];
			planes[i].distance_ = viewProjectionMatrix.m[3][2];
			break;
		case 5: // 遠平面
			planes[i].normal_.x = viewProjectionMatrix.m[0][3] - viewProjectionMatrix.m[0][2];
			planes[i].normal_.y = viewProjectionMatrix.m[1][3] - viewProjectionMatrix.m[1][2];
			planes[i].normal_.z = viewProjectionMatrix.m[2][3] - viewProjectionMatrix.m[2][2];
			planes[i].distance_ = viewProjectionMatrix.m[3][3] - viewProjectionMatrix.m[3][2];
			break;
		}

		// プレーンの法線ベクトルを正規化
		float length = planes[i].normal_.Length();
		planes[i].normal_ /= length;
		planes[i].distance_ /= length;
	}

	// オブジェクトの中心を取得
	Vector3 center = sphere.center_;

	// オブジェクトの中心がすべての視錐台平面内にあるかどうかをチェック
	for (int i = 0; i < 6; i++) {
		if (Dot(planes[i].normal_, center) + planes[i].distance_ + sphere.radius_ < 0) {
			// 中心が平面の裏側にある場合、視錐台外にあります
			return false;
		}
	}

	// すべての平面内に中心がある場合、オブジェクトは視錐台内にあります
	return true;
}
