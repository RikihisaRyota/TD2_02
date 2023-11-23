#include "BlockWorldTransform.h"

#include "MyMath.h"

void BlockWorldTransform::Initialize() {
	// scale
	scale_ = { 1.0f,1.0f,1.0f };
	// rotatition
	rotation_ = { 0.0f,0.0f,0.0f };
	// translation
	translate_ = { 0.0f,0.0f,0.0f };
	// matWorld
	matWorld_ = MakeIdentity4x4();
}

void BlockWorldTransform::UpdateMatrix() {
	Matrix4x4 matScale, matRot, matTrans;
	matScale = MakeIdentity4x4();
	matRot = MakeIdentity4x4();
	matTrans = MakeIdentity4x4();

	// スケール、回転、平行移動行列の計算
	matScale = MakeScaleMatrix(scale_);
	matRot *= MakeRotateXYZMatrix(rotation_);
	matTrans = MakeTranslateMatrix(translate_);

	// ワールド行列の合成
	matWorld_ = MakeIdentity4x4(); // 変形をリセット
	matWorld_ *= matScale;          // ワールド行列にスケーリングを反映
	matWorld_ *= matRot;            // ワールド行列に回転を反映
	matWorld_ *= matTrans;          // ワールド行列に平行移動を反映

	// ワールド座標の更新
	worldPos_.x = matWorld_.m[3][0];
	worldPos_.y = matWorld_.m[3][1];
	worldPos_.z = matWorld_.m[3][2];
}
