#pragma once

#include "Collider.h"
#include <list>

class CollisionManager
{
public:

	static CollisionManager* GetInstance();
	
	void Init();

	void Clear();

	void SetCollider(Collider* collider);

	void CheckCollision();

private:
	CollisionManager() = default;
	~CollisionManager() = default;
	CollisionManager(const CollisionManager&) = delete;
	const CollisionManager& operator=(const CollisionManager&) = delete;

	bool IsNeedCollision(Collider* a, Collider* b)const;

	bool IsMatchedConfig(Collider* a, Collider* b) const;

	bool IsCollision(Collider* a, Collider* b) const;

	bool IsCollisionBox2DBox2D(Collider* a, Collider* b) const;

	bool IsCollisionBox2DMapChip2D(Collider* a, Collider* b) const;

private:
	
	std::list<Collider*> colliders_;

};