#pragma once
#include "BaseColliderShapeType.h"
#include <memory>

class Collider {
public:

	Collider();

	struct EditInfo
	{
		enum EditEnumV2 {
			V2POS,
			V2VELOCITY,
			V2MASKPOS,
			V2COUNT,
		};

		void SetI32Info(uint32_t info);
		void SetPairIInfo(std::pair<int, int> info);

		uint32_t collisionMask_;
		std::vector<uint32_t> i32Info_;
		std::vector<std::pair<int, int>> pairIInfo_;

		std::vector<int> iParas_;
		std::vector<float> fParas_;
		std::vector<Vector2> v2Paras_;
		std::vector<Vector3> v3Paras_;
	};

	virtual void OnCollision() = 0;

	uint32_t GetCollisionAttribute() const;

	void SetCollisionAttribute(uint32_t attribute);

	uint32_t GetCollisionMask() const;

	void SetCollisionMask(uint32_t mask);

	std::unique_ptr<BaseColliderShapeType> shapeType_;

	EditInfo editInfo_;

public:

	uint32_t collisionAttribute_ = 0x00000000;

	uint32_t collisionMask_ = 0x00000000;

};