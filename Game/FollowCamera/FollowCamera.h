#pragma once
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <memory>
#include <string>

class FollowCamera {
public:

	FollowCamera();

	void Initialize();

	void Update();

	const ViewProjection& GetViewProjection() { return viewProjection_; }

	void SetTarget(const WorldTransform* target);

	void Reset();

private:

	void SetGlobalVariable();

	void ApplyGlobalVariable();

private:
	const WorldTransform* target_ = nullptr;

	ViewProjection viewProjection_;

	enum FParameterNames {
		kInterpolationRate, // カメラの遅延数値
		kMinPosY, 
		kCountFloatParameter, // 末尾
	};

	// 定数パラメータ配列
	float fParameters_[FParameterNames::kCountFloatParameter];

	std::string fParameterItemNames[FParameterNames::kCountFloatParameter] = {
		"カメラの遅延数値",
		"カメラのこれ以上下に行かないy座標"
	};

	enum V3ParameterNames {
		kOffset, // カメラ座標のズレ
		kCountV3Parameter, // 末尾
	};

	// 定数パラメータ配列
	Vector3 v3Parameters_[V3ParameterNames::kCountV3Parameter];

	std::string v3ParameterItemNames[V3ParameterNames::kCountV3Parameter] = {
		"カメラ座標のズレ",
	};

	// 追従対象の残像座標
	Vector3 interTarget_;

	const std::string groupName_ = "FollowCamera";
};
