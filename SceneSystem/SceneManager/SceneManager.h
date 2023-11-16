#pragma once

#include <memory>
#include "SceneSystem/IScene/IScene.h"

class SceneManager
{
private:
	// シーンを保持するメンバ変数
	std::unique_ptr<IScene> sceneArr_[4];

	// どのステージを呼び出すかを管理する変数
	int currentSceneNo_; // 現在のシーン
	int preSceneNo_; // 前のシーン

public:
	SceneManager();
	~SceneManager();

	int Run(); //この関数でゲームループを呼び出す
};

