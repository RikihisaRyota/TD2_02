#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include <list>
#include "Model.h"

class IBlock
{
public:

	virtual void Init() = 0;

	virtual void Update() = 0;

	virtual void OnCollision() = 0;

protected:

	bool isLife_;
};

class NormalBlock : public IBlock
{
public:
	NormalBlock();
	~NormalBlock();

	void Init();

	void Update();

	void OnCollision();

private:

};




