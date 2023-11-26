#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include <list>
#include "Model.h"

//class IBlock : public Collider
//{
//public:
//
//	virtual void Init(const Vector3& pos) = 0;
//
//	virtual void Update() = 0;
//
//protected:
//
//	Vector2 pos_;
//};

class OutBlock : public Collider
{
public:
	OutBlock(const Vector3& pos);

	void Update();

private:
	void OnCollision() override;

	void SetCollider();
};

class OutBlockManager 
{
public:
	static OutBlockManager* GetInstance();

	void Init();

	void CreateOutBlock(const Vector3& pos);

	void Update();

private:
	OutBlockManager() = default;
	~OutBlockManager() = default;
	OutBlockManager(const OutBlockManager&) = delete;
	const OutBlockManager& operator=(const OutBlockManager&) = delete;

private:

	std::list<std::unique_ptr<OutBlock>> outBlocks_;
};




