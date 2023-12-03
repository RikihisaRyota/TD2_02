#include "IScene.h"

// タイトルシーンで初期化
int IScene::sceneNo_ = TITLE;

int IScene::stageNo_ = 0;

IScene::~IScene(){}

int IScene::GetSceneNo() { return sceneNo_; }
