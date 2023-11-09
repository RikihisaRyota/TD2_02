#include "Player.h"

#include "Input.h"
#include "GlobalVariables/GlobalVariables.h"

Player::Player()
{
	models_.push_back(std::make_unique<Model>());
	models_[Parts::kMain].reset(Model::Create("Cube"));
	
	modelWorldTransforms_.push_back(WorldTransform());
	modelWorldTransforms_[Parts::kMain].Initialize();

	worldTransform_.Initialize();
	worldTransform_.translate_ = { 50.0f,20.0f,0.0f };
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	modelWorldTransforms_[Parts::kMain].parent_ = &worldTransform_;

	UpdateMatrix();

	isJump_ = true;
	velocity_ = {};
	parameters_[FloatParameterNames::kMoveSpeed] = 0.1f;
	parameters_[FloatParameterNames::kJumpInitialVelocity] = 1.0f;
	parameters_[FloatParameterNames::kGravity] = -0.02f;
	parameters_[FloatParameterNames::kFallingGravity] = -0.01f;

	SetGlobalVariable();
}

void Player::Initialize()
{
	statusRequest_ = Status::kNormal;

	worldTransform_.Initialize();
	worldTransform_.translate_ = { 50.0f,20.0f,0.0f };
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };

	UpdateMatrix();

	isJump_ = true;
	velocity_ = {};
}

void Player::UpdateMatrix()
{
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < Parts::kCountParts; i++) {
		modelWorldTransforms_[i].UpdateMatrix();
	}
}

void Player::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		globalVariables->AddItem(groupName_, floatParameterItemNames[i], parameters_[i]);
	}
	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		parameters_[i] = globalVariables->GetFloatValue(groupName_, floatParameterItemNames[i]);
	}
}

void Player::NormalInitialize()
{

}

void Player::NormalUpdate()
{
	Input* input = Input::GetInstance();

	if (velocity_.y == 0.0f) {
		isJump_ = false;
	}

	Vector3 move = { input->GetGamePadLStick().x,0.0f,0.0f };

	move.x *= parameters_[FloatParameterNames::kMoveSpeed];

	if (input->PressedGamePadButton(Input::GamePadButton::kA) && !isJump_) {
		isJump_ = true;
		move.y += parameters_[FloatParameterNames::kJumpInitialVelocity];
	}

	if (velocity_.y <= 0.0f) {
		move.y += parameters_[FloatParameterNames::kFallingGravity];
	}
	else {
		move.y += parameters_[FloatParameterNames::kGravity];
	}

	velocity_.x = 0.0f;
	velocity_ += move;

	worldTransform_.translate_ += velocity_;

}

void Player::Update()
{

	ApplyGlobalVariable();

	if (statusRequest_) {
		status_ = statusRequest_.value();

		switch (status_)
		{
		case Player::Status::kNormal:
			NormalInitialize();
			break;
		
		default:
			break;
		}

		statusRequest_ = std::nullopt;
	}

	switch (status_)
	{
	case Player::Status::kNormal:
		NormalUpdate();
		break;
	
	default:
		break;
	}

	UpdateMatrix();
}

void Player::Draw(const ViewProjection& viewProjection)
{
	for (int i = 0; i < Parts::kCountParts; i++) {
		models_[i]->Draw(modelWorldTransforms_[i], viewProjection);
	}
}


