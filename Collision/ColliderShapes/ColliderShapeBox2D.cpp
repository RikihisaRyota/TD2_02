#include "ColliderShapeBox2D.h"

bool ColliderShapeBox2D::IsCollisionBox2DBox2D(BaseColliderShapeType* a, BaseColliderShapeType* b)
{
	if (a->colliderType_ != BaseColliderShapeType::ColliderType::COLLIDER || b->colliderType_ != BaseColliderShapeType::ColliderType::COLLIDER) {
		if (IsCollisionBox2DBox2D(a->v2Paras_[ColliderShapeBox2D::v2Info::CENTER], a->v2Paras_[ColliderShapeBox2D::v2Info::SCALE],
			b->v2Paras_[ColliderShapeBox2D::v2Info::CENTER], b->v2Paras_[ColliderShapeBox2D::v2Info::SCALE])) {
			return true;
		}
	}
	else {
		// ここは本来めり込み処理を行いたい
		if (IsCollisionBox2DBox2D(a->v2Paras_[ColliderShapeBox2D::v2Info::CENTER], a->v2Paras_[ColliderShapeBox2D::v2Info::SCALE],
			b->v2Paras_[ColliderShapeBox2D::v2Info::CENTER], b->v2Paras_[ColliderShapeBox2D::v2Info::SCALE])) {
			return true;
		}
	}

	return false;
}

bool ColliderShapeBox2D::IsCollisionBox2DBox2D(const Vector2& pos0, const Vector2& scale0, const Vector2& pos1, const Vector2& scale1)
{
	Vector2 min0 = { pos0.x - scale0.x,pos0.y - scale0.y };
	Vector2 max0 = { pos0.x + scale0.x,pos0.y + scale0.y };

	Vector2 min1 = { pos1.x - scale1.x,pos1.y - scale1.y };
	Vector2 max1 = { pos1.x + scale1.x,pos1.y + scale1.y };

	if (min0.x <= max1.x && max0.x >= min1.x &&
		min0.y <= max1.y && max0.y >= min1.y) {
		return true;
	}

	return false;
}

ColliderShapeBox2D::ColliderShapeBox2D(BaseColliderShapeType::ColliderType type, Plane plane)
{

	shapeType_ = ShapeType::BOX2D;
	colliderType_ = type;
	plane_ = plane;

	for (int i = 0; i < v2Info::V2COUNT; i++) {
		v2Paras_.push_back(Vector2());
	}

}

void ColliderShapeBox2D::SetV2Info(const Vector3& center, const Vector3& scale, const Vector3& velocity)
{

	switch (plane_)
	{
	case ColliderShapeBox2D::Plane::XY:

		v2Paras_[v2Info::CENTER] = { center.x,center.y };
		v2Paras_[v2Info::SCALE] = { scale.x,scale.y };
		v2Paras_[v2Info::VELOCITY] = { velocity.x,velocity.y };

		break;
	case ColliderShapeBox2D::Plane::XZ:
		v2Paras_[v2Info::CENTER] = { center.x,center.z };
		v2Paras_[v2Info::SCALE] = { scale.x,scale.z };
		v2Paras_[v2Info::VELOCITY] = { velocity.x,velocity.z };

		break;
	case ColliderShapeBox2D::Plane::YZ:
		v2Paras_[v2Info::CENTER] = { center.y,center.z };
		v2Paras_[v2Info::SCALE] = { scale.y,scale.z };
		v2Paras_[v2Info::VELOCITY] = { velocity.y,velocity.z };
		break;
	default:
		break;
	}
}
