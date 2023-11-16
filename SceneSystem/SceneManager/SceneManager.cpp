#include "SceneManager.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "GameScene.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "./Engine/ShaderCompiler.h"
#include "SceneSystem/TitleScene/TitleScene.h"
#include "SceneSystem/SelectScene/SelectScene.h"
#include "SceneSystem/StageScene/StageScene.h"
#include "SceneSystem/ClearScene/ClearScene.h"

#include "GlobalVariables/GlobalVariables.h"

SceneManager::SceneManager()
{
	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[SELECT] = std::make_unique<SelectScene>();
	sceneArr_[STAGE] = std::make_unique<StageScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();

	IScene::sceneNo_ = TITLE;
	currentSceneNo_ = TITLE;
}

SceneManager::~SceneManager()
{
}

int SceneManager::Run()
{
	// メインループ
	while (true) {
		// メッセージ処理
		if (WinApp::GetInstance()->ProcessMessage()) {
			break;
		}
		// ImGui受付開始
		ImGuiManager::GetInstance()->Begin();
		// 入力関連の毎フレーム処理
		Input::GetInstance()->Update();
		// 音声アップデート
		Audio::GetInstance()->Update();

		GlobalVariables::GetInstance()->Update();
		// ゲームシーンの毎フレーム処理
		preSceneNo_ = currentSceneNo_;
		currentSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		if (preSceneNo_ != currentSceneNo_) {
			sceneArr_[currentSceneNo_]->Init();
		}
		sceneArr_[currentSceneNo_]->Update();
		// ImGui受付終了
		ImGuiManager::GetInstance()->End();
		// 描画開始
		DirectXCommon::GetInstance()->PreDraw();
		// ゲームシーンの描画
		sceneArr_[currentSceneNo_]->Draw();
		// 描画終わり
		DirectXCommon::GetInstance()->PostDraw();
		DirectXCommon::GetInstance()->PreUIDraw();
		// ImGui描画
		ImGuiManager::GetInstance()->Draw();
		DirectXCommon::GetInstance()->PostUIDraw();
		
	}


	return 0;
}
