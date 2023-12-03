#include "MiniGameManager.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"
#include <numbers>
#include <random>
#include "SceneSystem/IScene/IScene.h"

MiniGameManager* MiniGameManager::GetInstance()
{
	static MiniGameManager instance;
	return &instance;
}

void MiniGameManager::FirstInit()
{
	itemManager_ = ItemManager::GetInstance();
	needleManager_ = NeedleManager::GetInstance();
}

void MiniGameManager::Init()
{
	if (IScene::stageNo_ == IScene::MiniGameStageNo_) {
		countFrame_ = 0;
		itemManager_->MiniGameInit();
		needleManager_->Init();
	}
}

void MiniGameManager::Create()
{

	std::random_device rand;

	int nom = rand() % 11;

	Vector3 pos = { (rand() % 30 + 66) * 2.0f + 1.0f,50.0f,0.0f };

	if (nom <= 7) {
		itemManager_->MiniGameCreateItem(pos);
	}
	else {
		needleManager_->CreateNeadle(pos);
	}
}


void MiniGameManager::Update()
{
	if (IScene::stageNo_ == IScene::MiniGameStageNo_) {
		if (countFrame_ < 60 * 15 && countFrame_ % 120 == 0) {
			Create();
		}
		else if (countFrame_ >= 60 * 15 && countFrame_ < 60 * 30 && countFrame_ % 110 == 0) {
			Create();
		}
		else if (countFrame_ >= 60 * 30 && countFrame_ < 60 * 45 && countFrame_ % 100 == 0) {
			Create();
			Create();
		}

		countFrame_++;
	}
}
