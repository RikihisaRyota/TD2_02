#include "Block.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "ModelManager.h"
#include "GlobalVariables/GlobalVariables.h"

OutBlock::OutBlock(const Vector3& pos)
{
	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributeOut);
	SetCollisionMask(kCollisionAttributePlayer);

	shapeType_->SetV2Info(Vector2{ pos.x,pos.y }, Vector2{ 1.001f,0.8f }, Vector2{ 0.0f,0.0f });
}

void OutBlock::Update()
{
	SetCollider();
}

void OutBlock::OnCollision()
{

}

void OutBlock::SetCollider()
{
	CollisionManager::GetInstance()->SetCollider(this);
}

OutBlockManager* OutBlockManager::GetInstance()
{
	static OutBlockManager instance;
	return &instance;
}

void OutBlockManager::Init()
{
	outBlocks_.clear();
}

void OutBlockManager::CreateOutBlock(const Vector3& pos)
{
	outBlocks_.emplace_back(std::make_unique<OutBlock>(pos));
}

void OutBlockManager::Update()
{
	for (const std::unique_ptr<OutBlock>& outBlock : outBlocks_) {
		outBlock->Update();
	}
}

MarkBlock::MarkBlock(const Vector3& pos)
{
	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionAttributePlayer);

	shapeType_->SetV2Info(Vector2{ pos.x,pos.y }, Vector2{ 1.0f,1.0f }, Vector2{ 0.0f,0.0f });

	// main.cppのモデルマネージャーの番号
	model_.reset(ModelManager::GetInstance()->GetBlockModel(17));

	worldTransform_.Initialize();
	worldTransform_.translate_ = pos;
	worldTransform_.UpdateMatrix();
	pos_ = pos;

	isSet_ = true;
	isChangeScale_ = false;

	SetGlobalVariable();
}

void MarkBlock::Init(const Vector3& pos)
{
	worldTransform_.Reset();
	worldTransform_.translate_ = pos;
	worldTransform_.UpdateMatrix();
	pos_ = pos;

	isSet_ = true;
	isChangeScale_ = false;
}

void MarkBlock::Update()
{
	ApplyGlobalVariable();

	if (!isChangeScale_) {
		SetCollider();
	}

	worldTransform_.UpdateMatrix();
}

void MarkBlock::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}

void MarkBlock::Reset()
{
	isSet_ = false;
}

void MarkBlock::OnCollision()
{
	isChangeScale_ = true;
}

void MarkBlock::SetCollider()
{
	CollisionManager::GetInstance()->SetCollider(this);
}

void MarkBlock::SetGlobalVariable()
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

void MarkBlock::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

MarkBlockManager* MarkBlockManager::GetInstance()
{
	static MarkBlockManager instance;
	return &instance;
}

void MarkBlockManager::Init()
{

}

void MarkBlockManager::CreateMarkBlock(const Vector3& pos)
{
}

void MarkBlockManager::Update()
{
}

void MarkBlockManager::Draw(const ViewProjection& viewProjection)
{
}

void MarkBlockManager::Clear()
{
}
