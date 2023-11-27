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
#include "ParticleManager.h"
#include "ParticleUIManager.h"
#include "Game/StageData/StageData.h"

#include "GlobalVariables/GlobalVariables.h"
#include "Game/Nedle/Needle.h"

SceneManager::SceneManager() {
	IScene::sceneNo_ = TITLE;
	currentSceneNo_ = IScene::sceneNo_;
	currentSceneChangeSceneNo_ = currentSceneNo_;
	IScene::stageNo_ = 0;

	sceneArr_[TITLE] = std::make_unique<TitleScene>();
	sceneArr_[SELECT] = std::make_unique<SelectScene>();
	sceneArr_[STAGE] = std::make_unique<StageScene>();
	sceneArr_[CLEAR] = std::make_unique<ClearScene>();

	sceneArr_[currentSceneNo_]->Init();

	soundManager_ = std::make_unique<SoundManager>();

	sceneChange_ = std::make_unique<SceneChange>();
}

SceneManager::~SceneManager() {}

int SceneManager::Run() {
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

		DirectXCommon::GetInstance()->Update();

#ifdef _DEBUG
		ImGui::Begin("fps");
		ImGui::Text("Frame rate: %3.0f fps", ImGui::GetIO().Framerate);
		ImGui::Text("Delta Time: %.4f", ImGui::GetIO().DeltaTime);
		ImGui::End();
#endif // _DEBUG

		StageData::Update();
		// ゲームシーンの毎フレーム処理
		preSceneChangeSceneNo_ = currentSceneChangeSceneNo_;
		currentSceneChangeSceneNo_ = sceneArr_[currentSceneNo_]->GetSceneNo();
		if (preSceneChangeSceneNo_ != currentSceneChangeSceneNo_) {
			soundManager_->SetScene(preSceneChangeSceneNo_, currentSceneChangeSceneNo_);
			ParticleManager::GetInstance()->Initialize();
			ParticleUIManager::GetInstance()->Initialize();
			// シーン遷移開始
			sceneChange_->SetSceneChange(true);
		}
		// シーン遷移していなかったら
		if (!sceneChange_->GetSceneChange()) {
			sceneArr_[currentSceneNo_]->Update();
		}
		else {
			sceneChange_->Update();
			if (sceneChange_->GetInitialize()) {
				currentSceneNo_ = currentSceneChangeSceneNo_;
				preSceneNo_ = preSceneChangeSceneNo_;
				sceneArr_[currentSceneNo_]->Init();
				soundManager_->Initialize();
				sceneChange_->SetInitialize(false);
			}
		}

		// ImGui受付終了
		ImGuiManager::GetInstance()->End();
		// 描画開始
		DirectXCommon::GetInstance()->PreDraw();
		// ゲームシーンの描画
		sceneArr_[currentSceneNo_]->Draw();
		// 描画終わり
		DirectXCommon::GetInstance()->PostDraw();
		DirectXCommon::GetInstance()->PreUIDraw();
		sceneArr_[currentSceneNo_]->UIDraw();
		// シーン遷移の描画
		sceneChange_->Draw();
		// ImGui描画
#ifdef _DEBUG
		ImGuiManager::GetInstance()->Draw();
#endif // DEBUG
		DirectXCommon::GetInstance()->PostUIDraw();

		soundManager_->Update();
	}

	return 0;
}
