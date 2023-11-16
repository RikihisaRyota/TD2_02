#include "Collider.h"

Collider::Collider()
{
	collisionAttribute_ = 0x00000000;

	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}
}

uint32_t Collider::GetCollisionAttribute() const
{
	return collisionAttribute_;
}

void Collider::SetCollisionAttribute(uint32_t attribute)
{
	collisionAttribute_ = collisionAttribute_ | attribute;
}

uint32_t Collider::GetCollisionMask() const
{
	return collisionMask_;
}

void Collider::SetCollisionMask(uint32_t mask)
{
	collisionMask_ = collisionMask_ | mask;
}

void Collider::EditInfo::SetI32Info(uint32_t info)
{
	for (uint32_t num : i32Info_) {
		if (num == info) {
			return;
		}
	}

	i32Info_.push_back(info);
}

void Collider::EditInfo::SetPairIInfo(std::pair<int, int> info)
{
	for (std::pair<int, int> num : pairIInfo_) {
		if (num == info) {
			return;
		}
	}

	pairIInfo_.push_back(info);
}
