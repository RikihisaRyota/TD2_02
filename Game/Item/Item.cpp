#include "Item.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"
#include "MapChip.h"

Item::Item(const Vector3& pos)
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
	worldTransform_.translate_ = pos;
	pos_ = worldTransform_.translate_;
	worldTransform_.UpdateMatrix();

	isLife_ = true;
	velocity_ = {};

	SetGlobalVariable();
}

Item::~Item()
{
	model_.release();
}

void Item::Init(const Vector3& pos)
{
	worldTransform_.Reset();
	worldTransform_.translate_ = pos;
	pos_ = worldTransform_.translate_;
	worldTransform_.UpdateMatrix();
}

void Item::Update()
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

void Item::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}

void Item::OnCollision()
{

	isLife_ = false;

}

void Item::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ 0.8f,0.8f }, Vector2{ velocity_.x,velocity_.y });

	CollisionManager::GetInstance()->SetCollider(this);
}

void Item::SetGlobalVariable()
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

void Item::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void Item::CreateInit()
{
	countFrame_ = 0;
}

void Item::CreateUpdate()
{

	worldTransform_.translate_ = Ease::UseEase(pos_, { pos_.x,pos_.y - 1.9f,pos_.z },
		countFrame_, iInfo_[IInfoNames::kCreatFrame], Ease::EaseType::EaseOutSine);

	if (countFrame_ >= iInfo_[IInfoNames::kCreatFrame]) {
		stateRequest_ = State::kFalling;
	}

}

void Item::FallingInit()
{
	SetCollisionMask(kCollisionAttributeBlock);
	velocity_ = {};
}

void Item::FallingUpdate()
{
	velocity_.y += fInfo_[FInfoNames::kGravity];

	if (velocity_.y <= -fInfo_[FInfoNames::kMaxSpeed]) {
		velocity_.y = -fInfo_[FInfoNames::kMaxSpeed];
	}

	worldTransform_.translate_ += velocity_;
}

void (Item::* Item::spStateInitFuncTable[])() {
	&Item::CreateInit,
	&Item::FallingInit,
};

void (Item::* Item::spStateUpdateFuncTable[])() {
	&Item::CreateUpdate,
	&Item::FallingUpdate,
};

ItemManager* ItemManager::GetInstance()
{
	static ItemManager instance;
	return &instance;
}

void ItemManager::Init()
{
	Clear();
	SetGlobalVariable();
	countFrame_ = 0;
}

bool ItemManager::IsCreatNedle()
{
	if (countFrame_ >= iInfo_[IInfoNames::kCreatIntervalFrame]) {
		countFrame_ = 0;
		return true;
	}

	return false;
}

void ItemManager::CreateItem(const Vector3& pos)
{
	items_.push_back(std::make_unique<Item>(pos));
}

void ItemManager::Clear()
{
	for (const std::unique_ptr<Item>& item : items_) {
		item->SetIsLife(false);
	}
}

void ItemManager::SetItem(const Vector3& pos)
{
	for (const std::unique_ptr<Item>& item : items_) {
		if (!item->IsLife()) {
			item->Init(pos);
			break;
		}
	}
}

void ItemManager::Update()
{
	countFrame_++;

	items_.remove_if([](std::unique_ptr<Item>& nedle) {
		if (!nedle->IsLife()) {
			nedle.reset();
			nedle = nullptr;
			return true;
		}
		return false;
	});

	for (const std::unique_ptr<Item>& nedle : items_) {
		nedle->Update();
	}
}

void ItemManager::Draw(const ViewProjection& viewProjection)
{
	for (const std::unique_ptr<Item>& nedle : items_) {
		nedle->Draw(viewProjection);
	}
}

void ItemManager::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}

	ApplyGlobalVariable();
}

void ItemManager::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}
}
