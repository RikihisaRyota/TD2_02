#include "Player.h"

#include "Input.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"

Player::Player()
{

	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::RIGID_BODY);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeBlock);

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
	isRight_ = true;
	velocity_ = {};
	parameters_[FloatParameterNames::kMoveSpeed] = 0.1f;
	parameters_[FloatParameterNames::kJumpInitialVelocity] = 1.0f;
	parameters_[FloatParameterNames::kGravity] = -0.02f;
	parameters_[FloatParameterNames::kFallingGravity] = -0.01f;

	v3Parameters_[V3ParameterNames::kInitialPos] = { 50.0f,20.0f,0.0f };
	preInitialPos_ = v3Parameters_[V3ParameterNames::kInitialPos];

	SetGlobalVariable();
}

void Player::Initialize()
{
	statusRequest_ = Status::kNormal;

	worldTransform_.translate_ = v3Parameters_[kInitialPos];
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

void Player::OnCollision()
{

	worldTransform_.translate_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].x;
	worldTransform_.translate_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].y;

	velocity_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x;
	velocity_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y;

	UpdateMatrix();
}

void Player::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y }, 
		Vector2{ worldTransform_.scale_.x,worldTransform_.scale_.y },Vector2{ velocity_.x,velocity_.y });
}

void Player::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		globalVariables->AddItem(groupName_, floatParameterItemNames[i], parameters_[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		globalVariables->AddItem(groupName_, v3ParameterItemNames[i], v3Parameters_[i]);
	}

	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		parameters_[i] = globalVariables->GetFloatValue(groupName_, floatParameterItemNames[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		v3Parameters_[i] = globalVariables->GetVector3Value(groupName_, v3ParameterItemNames[i]);
	}

	if (preInitialPos_ != v3Parameters_[V3ParameterNames::kInitialPos]) {
		preInitialPos_ = v3Parameters_[V3ParameterNames::kInitialPos];
		worldTransform_.translate_ = v3Parameters_[V3ParameterNames::kInitialPos];
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

void Player::JumpInitialize()
{
	velocity_ = {};

}

void Player::JumpUpdate()
{




}

void Player::GripWallInitialize()
{
	velocity_ = {};

}

void Player::GripWallUpdate()
{

	if (isRight_) {
		// 右の壁


	}
	else {
		// 左の壁

	}

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

	SetCollider();
}

void Player::Draw(const ViewProjection& viewProjection)
{
	for (int i = 0; i < Parts::kCountParts; i++) {
		models_[i]->Draw(modelWorldTransforms_[i], viewProjection);
	}
}


