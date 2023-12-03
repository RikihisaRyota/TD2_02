#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include <list>
#include "Model.h"
#include <array>
#include <optional>
#include "Game/Item/Item.h"
#include "Game/Nedle/Needle.h"

class MiniGameManager
{
public:
	static MiniGameManager* GetInstance();

	void FirstInit();

	void Init();

	void Create();

	void Update();

private:
	MiniGameManager() = default;
	~MiniGameManager() = default;
	MiniGameManager(const MiniGameManager&) = delete;
	const MiniGameManager& operator=(const MiniGameManager&) = delete;

	//void SetGlobalVariable();

	//void ApplyGlobalVariable();

private:
	

	int countFrame_;

	ItemManager* itemManager_ = nullptr;
	NeedleManager* needleManager_ = nullptr;
};
