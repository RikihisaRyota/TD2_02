#include "ColliderShapeMapChip2D.h"
#include "ColliderShapeBox2D.h"

ColliderShapeMapChip2D::ColliderShapeMapChip2D(std::vector<std::vector<uint32_t>> mapChip, uint32_t kMaxHeightChipNum, const Vector3& bottomLeftPos,
	const Vector3& chipSize, BaseColliderShapeType::ColliderType type, Plane plane)
{
	shapeType_ = ShapeType::MAPCHIP2D;
	colliderType_ = type;
	plane_ = plane;

	mapChip2D_.mapChip_ = mapChip;

	for (int i = 0; i < iInfo::ICOUNT; i++) {
		iParas_.push_back(0);
	}

	iParas_[iInfo::MAXHEIGHTCHIPNUM] = kMaxHeightChipNum;

	for (int i = 0; i < v2Info::V2COUNT; i++) {
		v2Paras_.push_back(Vector2());
	}

	switch (plane)
	{
	case ColliderShapeMapChip2D::Plane::XY:

		v2Paras_[v2Info::BOTTOMLEFTPOS] = { bottomLeftPos.x,bottomLeftPos.y };
		v2Paras_[v2Info::CHIPSIZE] = { chipSize.x,chipSize.y };

		break;
	case ColliderShapeMapChip2D::Plane::XZ:
		v2Paras_[v2Info::BOTTOMLEFTPOS] = { bottomLeftPos.x,bottomLeftPos.z };
		v2Paras_[v2Info::CHIPSIZE] = { chipSize.x,chipSize.z };

		break;
	case ColliderShapeMapChip2D::Plane::YZ:
		v2Paras_[v2Info::BOTTOMLEFTPOS] = { bottomLeftPos.y,bottomLeftPos.z };
		v2Paras_[v2Info::CHIPSIZE] = { chipSize.y,chipSize.z };

		break;
	default:
		break;
	}

	/*for (int y = 0; y < mapChip.size(); y++) {

		mapChip2D_.shape_.push_back(std::vector < std::unique_ptr<BaseColliderShapeType>>());

		float posY = (kMaxHeightChipNum - 1 - y) * (chipSize.y * 2) + chipSize.y + v2Paras_[v2Info::BOTTOMLEFTPOS].y;

		for (int x = 0; x < mapChip[y].size(); x++) {

			float posX = chipSize.x + x * chipSize.x * 2 + v2Paras_[v2Info::BOTTOMLEFTPOS].x;

			mapChip2D_.shape_[y].push_back(std::make_unique<ColliderShapeBox2D>());

			mapChip2D_.shape_[y][x]->
		}
	}*/

}

void ColliderShapeMapChip2D::SetNoRigitBody(int no)
{
	for (int noR : mapChip2D_.noRigitBody_) {
		if (noR == no) {
			return;
		}
	}

	mapChip2D_.noRigitBody_.push_back(no);
}

void ColliderShapeMapChip2D::SetNoCollider(int no)
{
	for (int noR : mapChip2D_.noCollider_) {
		if (noR == no) {
			return;
		}
	}

	mapChip2D_.noCollider_.push_back(no);
}

bool ColliderShapeMapChip2D::IsRigitBody(int y, int x)
{
	for (int no : mapChip2D_.noRigitBody_) {
		if (mapChip2D_.mapChip_[y][x] == no) {
			return true;
		}
	}

	return false;
}

bool ColliderShapeMapChip2D::IsCollider(int y, int x)
{
	for (int no : mapChip2D_.noCollider_) {
		if (mapChip2D_.mapChip_[y][x] == no) {
			return true;
		}
	}

	return false;
}
