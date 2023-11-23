#include "Nedle.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"

Needle::Needle(const Vector3& position)
{
	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributeOut);
	SetCollisionMask(kCollisionAttributePlayer);

	model_.reset(ModelManager::GetInstance()->GetModel("player"));

	worldTransform_.Initialize();
	worldTransform_.translate_ = position;
	pos_ = worldTransform_.translate_;
	worldTransform_.UpdateMatrix();

	isLife_ = true;
	velocity_ = {};

	SetGlobalVariable();
}

Needle::~Needle()
{
	model_.release();
}

void Needle::Update()
{
	countFrame_++;

	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		stateRequest_ = std::nullopt;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();

	worldTransform_.UpdateMatrix();

	SetCollider();
}

void Needle::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}

void Needle::OnCollision()
{

	isLife_ = false;

}

void Needle::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ 0.8f,0.8f }, Vector2{ velocity_.x,velocity_.y });

	CollisionManager::GetInstance()->SetCollider(this);
}

void Needle::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	ApplyGlobalVariable();
}

void Needle::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void Needle::CreateInit()
{
	countFrame_ = 0;
}

void Needle::CreateUpdate()
{

	worldTransform_.translate_ = Ease::UseEase(pos_, { pos_.x,pos_.y - 1.9f,pos_.z },
		countFrame_, iInfo_[IInfoNames::kCreatFrame], Ease::EaseType::EaseOutSine);

	if (countFrame_ >= iInfo_[IInfoNames::kCreatFrame]) {
		stateRequest_ = State::kFalling;
	}

}

void Needle::FallingInit()
{
	SetCollisionMask(kCollisionAttributeBlock);
	velocity_ = {};
}

void Needle::FallingUpdate()
{
	velocity_.y += fInfo_[FInfoNames::kGravity];

	if (velocity_.y <= -fInfo_[FInfoNames::kMaxSpeed]) {
		velocity_.y = -fInfo_[FInfoNames::kMaxSpeed];
	}

	worldTransform_.translate_ += velocity_;
}

void (Needle::* Needle::spStateInitFuncTable[])() {
	&Needle::CreateInit,
	&Needle::FallingInit,
};

void (Needle::* Needle::spStateUpdateFuncTable[])() {
	&Needle::CreateUpdate,
	&Needle::FallingUpdate,
};

NeedleManager* NeedleManager::GetInstance()
{
	static NeedleManager instance;
	return &instance;
}

void NeedleManager::Init()
{
	Clear();
	SetGlobalVariable();
	countFrame_ = 0;
}

bool NeedleManager::IsCreatNedle()
{
	if (countFrame_ >= iInfo_[IInfoNames::kCreatIntervalFrame]) {
		countFrame_ = 0;
		return true;
	}

	return false;
}

void NeedleManager::CreateNeadle(const Vector3& position)
{
	needles_.push_back(std::make_unique<Needle>(position));
}

void NeedleManager::Clear()
{
	needles_.clear();
}

void NeedleManager::Update()
{
	countFrame_++;

	needles_.remove_if([](std::unique_ptr<Needle>& nedle) {
		if (!nedle->IsLife()) {
			nedle.reset();
			nedle = nullptr;
			return true;
		}
		return false;
	});

	for (const std::unique_ptr<Needle>& nedle : needles_) {
		nedle->Update();
	}
}

void NeedleManager::Draw(const ViewProjection& viewProjection)
{
	for (const std::unique_ptr<Needle>& nedle : needles_) {
		nedle->Draw(viewProjection);
	}
}

void NeedleManager::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}

	ApplyGlobalVariable();
}

void NeedleManager::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}
}
