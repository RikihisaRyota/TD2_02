#pragma once
#include "Collision/BaseColliderShapeType.h"

class ColliderShapeMapChip2D : public BaseColliderShapeType
{
public:

	enum class Plane
	{
		XY,
		XZ,
		YZ
	};

	enum iInfo {
		MAXHEIGHTCHIPNUM,
		ICOUNT,
	};

	enum v2Info
	{
		BOTTOMLEFTPOS,
		CHIPSIZE,
		V2COUNT,
	};

	ColliderShapeMapChip2D(std::vector<std::vector<uint32_t>> mapChip, uint32_t kMaxHeightChipNum, const Vector3& bottomLeftPos, const Vector3& chipSize,
		BaseColliderShapeType::ColliderType type = BaseColliderShapeType::ColliderType::UNKNOWN, Plane plane = Plane::XY);

	void SetNoRigitBody(int no);

	void SetNoCollider(int no);

	bool IsRigitBody(int y, int x);

	bool IsCollider(int y, int x);

public:

	Plane plane_;
};
