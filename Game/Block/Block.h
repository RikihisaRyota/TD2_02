#pragma once
#include "ViewProjection.h"

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

class HogeBlock : public IBlock
{
public:
	HogeBlock();
	~HogeBlock();

	void Init();

	void Update();

	void OnCollision();

private:

};
