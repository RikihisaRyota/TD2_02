#pragma once

#include "Vector3.h"
#include "Matrix4x4.h"


struct BlockWorldTransform {
	// scale
	Vector3 scale_;
	// rotatition
	Vector3 rotation_;
	// translation
	Vector3 translate_;
	// matWorld
	Matrix4x4 matWorld_;
	// ワールド座標
	Vector3 worldPos_;

	void Initialize();
	void UpdateMatrix();

};