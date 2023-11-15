#include "FollowCamera.h"
#define DIRECTINPUT_VERSION 0x0800 
#include <dinput.h>
#include "MyMath.h"
#include "GlobalVariables/GlobalVariables.h"

FollowCamera::FollowCamera()
{
	viewProjection_.Initialize();

	interTarget_ = {};

	fParameters_[kInterpolationRate] = 0.1f;

	v3Parameters_[kOffset] = { 0.0f,2.0f,-44.5f };

	SetGlobalVariable();

	Reset();

}

void FollowCamera::Initialize() 
{
	
	Reset();

	viewProjection_.UpdateMatrix();
}

void FollowCamera::Update() {

	ApplyGlobalVariable();

	if (target_) {
		interTarget_ = Lerp(interTarget_, target_->worldPos_, fParameters_[kInterpolationRate]);

		viewProjection_.translate_ = interTarget_ + v3Parameters_[kOffset];
	}

	viewProjection_.UpdateMatrix();

}

void FollowCamera::SetTarget(const WorldTransform* target)
{
	target_ = target;
	Reset();
}

void FollowCamera::Reset()
{

	// 追従対象がいれば
	if (target_) {
		// 追従座標の初期化
		interTarget_ = target_->worldPos_;
	}

	viewProjection_.translate_ = interTarget_;

}

void FollowCamera::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < FParameterNames::kCountFloatParameter; i++) {
		globalVariables->AddItem(groupName_, fParameterItemNames[i], fParameters_[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		globalVariables->AddItem(groupName_, v3ParameterItemNames[i], v3Parameters_[i]);
	}

	ApplyGlobalVariable();
}

void FollowCamera::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < FParameterNames::kCountFloatParameter; i++) {
		fParameters_[i] = globalVariables->GetFloatValue(groupName_, fParameterItemNames[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		v3Parameters_[i] = globalVariables->GetVector3Value(groupName_, v3ParameterItemNames[i]);
	}
}

