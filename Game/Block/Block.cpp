#include "Block.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"

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
