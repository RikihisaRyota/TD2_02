#pragma once

#include <memory>
#include "SceneSystem/IScene/IScene.h"
#include "Game/Sound/SoundManager.h"
#include "SceneSystem/SceneChange/SceneChange.h"

class SceneManager
{
private:
	// シーンを保持するメンバ変数
	std::unique_ptr<IScene> sceneArr_[4];

	// どのステージを呼び出すかを管理する変数
	int currentSceneNo_; // 現在のシーン
	int preSceneNo_; // 前のシーン

	int currentSceneChangeSceneNo_;
	int preSceneChangeSceneNo_;

	// 音
	std::unique_ptr<SoundManager> soundManager_;
	std::unique_ptr<SceneChange> sceneChange_;
public:
	SceneManager();
	~SceneManager();

	int Run(); //この関数でゲームループを呼び出す
};

