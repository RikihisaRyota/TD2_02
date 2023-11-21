#include "Nedle.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"

Nedle::Nedle(const WorldTransform& worldTransform)
{
	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionAttributePlayer);

	model_.reset(ModelManager::GetInstance()->GetModel("player"));

	worldTransform_.Initialize();
	worldTransform_.translate_ = worldTransform.translate_;
	pos_ = worldTransform_.translate_;
	worldTransform_.UpdateMatrix();

	isLife_ = true;
	velocity_ = {};

	SetGlobalVariable();
}

Nedle::~Nedle()
{
	model_.release();
}

void Nedle::Update()
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

void Nedle::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}

void Nedle::OnCollision()
{

	isLife_ = false;

}

void Nedle::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ 0.8f,0.8f }, Vector2{ velocity_.x,velocity_.y });

	CollisionManager::GetInstance()->SetCollider(this);
}

void Nedle::SetGlobalVariable()
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

void Nedle::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void Nedle::CreateInit()
{
	countFrame_ = 0;
}

void Nedle::CreateUpdate()
{

	worldTransform_.translate_ = Ease::UseEase(pos_, { pos_.x,pos_.y - 1.9f,pos_.z },
		countFrame_, iInfo_[IInfoNames::kCreatFrame], Ease::EaseType::EaseOutSine);

	if (countFrame_ >= iInfo_[IInfoNames::kCreatFrame]) {
		stateRequest_ = State::kFalling;
	}

}

void Nedle::FallingInit()
{
	SetCollisionMask(kCollisionAttributeBlock);
	velocity_ = {};
}

void Nedle::FallingUpdate()
{
	velocity_.y += fInfo_[FInfoNames::kGravity];

	if (velocity_.y <= -fInfo_[FInfoNames::kMaxSpeed]) {
		velocity_.y = -fInfo_[FInfoNames::kMaxSpeed];
	}

	worldTransform_.translate_ += velocity_;
}

void (Nedle::* Nedle::spStateInitFuncTable[])() {
	&Nedle::CreateInit,
	&Nedle::FallingInit,
};

void (Nedle::* Nedle::spStateUpdateFuncTable[])() {
	&Nedle::CreateUpdate,
	&Nedle::FallingUpdate,
};

NedleManager* NedleManager::GetInstance()
{
	static NedleManager instance;
	return &instance;
}

void NedleManager::Init()
{
	Clear();
	SetGlobalVariable();
	countFrame_ = 0;
}

bool NedleManager::IsCreatNedle()
{
	if (countFrame_ >= iInfo_[IInfoNames::kCreatIntervalFrame]) {
		countFrame_ = 0;
		return true;
	}

	return false;
}

void NedleManager::CreateNeadle(const WorldTransform& worldTransform)
{
	nedles_.push_back(std::make_unique<Nedle>(worldTransform));
}

void NedleManager::Clear()
{
	nedles_.clear();
}

void NedleManager::Update()
{
	countFrame_++;

	nedles_.remove_if([](std::unique_ptr<Nedle>& nedle) {
		if (!nedle->IsLife()) {
			nedle.reset();
			nedle = nullptr;
			return true;
		}
		return false;
	});

	for (const std::unique_ptr<Nedle>& nedle : nedles_) {
		nedle->Update();
	}
}

void NedleManager::Draw(const ViewProjection& viewProjection)
{
	for (const std::unique_ptr<Nedle>& nedle : nedles_) {
		nedle->Draw(viewProjection);
	}
}

void NedleManager::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}

	ApplyGlobalVariable();
}

void NedleManager::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}
}
