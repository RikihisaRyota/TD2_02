#include "BaseColliderShapeType.h"

#include "Collision/ColliderShapes/ColliderShapeBox2D.h"

BaseColliderShapeType::ShapeType BaseColliderShapeType::getShapeType() const
{
	return shapeType_;
}

BaseColliderShapeType::ColliderType BaseColliderShapeType::GetColliderType() const
{
	return colliderType_;
}

void BaseColliderShapeType::SetColliderType(ColliderType type)
{
	colliderType_ = type;
}

void BaseColliderShapeType::SetV2Info(const Vector2& center, const Vector2& scale, const Vector2& velocity)
{

	v2Paras_[ColliderShapeBox2D::v2Info::CENTER] = center;
	v2Paras_[ColliderShapeBox2D::v2Info::SCALE] = scale;
	v2Paras_[ColliderShapeBox2D::v2Info::VELOCITY] = velocity;
}

void BaseColliderShapeType::MAPCHIP2D::SetMapChip(const std::vector<std::vector<uint32_t>>& mapchip)
{
	mapChip_ = mapchip;
}

void BaseColliderShapeType::MAPCHIP2D::SetNoRigitBody(int no)
{
	for (int noR : noRigitBody_) {
		if (noR == no) {
			return;
		}
	}

	noRigitBody_.push_back(no);
}

void BaseColliderShapeType::MAPCHIP2D::SetNoCollider(int no)
{
	for (int noR : noCollider_) {
		if (noR == no) {
			return;
		}
	}

	noCollider_.push_back(no);
}

bool BaseColliderShapeType::MAPCHIP2D::IsRigitBody(int y, int x)
{
	for (int no : noRigitBody_) {
		if (mapChip_[y][x] == no) {
			return true;
		}
	}

	return false;
}

bool BaseColliderShapeType::MAPCHIP2D::IsCollider(int y, int x)
{
	for (int no : noCollider_) {
		if (mapChip_[y][x] == no) {
			return true;
		}
	}

	return false;
}


