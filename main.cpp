#include "WinApp.h"
#include "DirectXCommon.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "ParticleGraphicsPipline.h"
#include "ParticleManager.h"
#include "ModelManager.h"
#include "ShaderCompiler.h"

#include "GlobalVariables/GlobalVariables.h"

#include <dxgidebug.h>
#include <cassert>

#pragma comment(lib,"dxguid.lib")

#include "ResourceLeakChecker.h"
#include "SceneSystem/SceneManager/SceneManager.h"
#include "Input.h"
#include "Sprite.h"
#include "LineGraphicsPipline.h"
#include "PlaneRenderer.h"
#include "OBJ.h"
#include "CubeRenderer.h"
#include "SphereRenderer.h"
#include "PrimitiveDrawer.h"
#include "Audio.h"

static ResourceLeackChecker leakCheck;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// ゲームウィンドウの作成
	WinApp* win = nullptr;
	win = WinApp::GetInstance();
	win->CreateGameWindow(L"DirectXClass");

	// ShderCompiler
	ShaderCompiler::Initialize();

	// DirectXの初期化処理
	DirectXCommon* dxCommon = nullptr;
	dxCommon = DirectXCommon::GetInstance();
	dxCommon->Initialize(win);

	// 入力の初期化
	Input* input = nullptr;
	input = Input::GetInstance();
	input->Initialize();

	// テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon);

	// スプライト静的初期化
	Sprite::StaticInitialize(dxCommon->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// BasicGraphicsPipelineの静的初期化
	BasicGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// ToonGraphicsPipelineの静的初期化
	ToonGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// LineGraphicsPipelineの静的初期化
	LineGraphicsPipline::SetDevice(dxCommon->GetDevice());

	// ParticleGraphicsPipelineの静的初期化
	ParticleGraphicsPipeline::SetDevice(dxCommon->GetDevice());

	// PostEffectPipeLine
	//PostEffectGraphicsPipeline::SetDevice(dxCommon->GetDevice());

	// Planeの静的初期化
	PlaneRenderer::SetDevice(dxCommon->GetDevice());

	// OBJの静的初期化
	OBJ::SetDevice(dxCommon->GetDevice());

	// Cubeのデバイスセット
	CubeRenderer::SetDevice(dxCommon->GetDevice());

	// Sphereのデバイスセット
	SphereRenderer::SetDevice(dxCommon->GetDevice());

	// Material
	Material::SetDevice(dxCommon->GetDevice());

	// Mesh
	Mesh::SetDevice(dxCommon->GetDevice());

	// Model
	Model::SetDevice(dxCommon->GetDevice());

	// Line
	PrimitiveDrawer::SetDevice(dxCommon->GetDevice());
	PrimitiveDrawer::GetInstance()->Initialize();

	// ImGuiの初期化
	ImGuiManager* imguiManager = ImGuiManager::GetInstance();
	imguiManager->Initialize(win, dxCommon);

	// Audio
	Audio* audio = Audio::GetInstance();
	audio->Initialize();

	// パーティクル
	ParticleManager::GetInstance()->Initialize();

	// モデル
	ModelManager::GetInstance()->LoadModel(
		{
			"player",
			"background",
		}
	);
	ModelManager::GetInstance()->LoadBlockModel(
		{
		"block",
		"redBlock",
		}
	);

	GlobalVariables::GetInstance()->LoadFiles();

	std::unique_ptr<SceneManager> sceneManager = std::make_unique<SceneManager>();
	sceneManager->Run();

	//// ゲームシーンの初期化
	//GameScene* gameScene = nullptr;
	//gameScene = new GameScene();
	//gameScene->Initialize();


	//// メインループ
	//while (true) {
	//	// メッセージ処理
	//	if (win->ProcessMessage()) {
	//		break;
	//	}
	//	// ImGui受付開始
	//	imguiManager->Begin();
	//	// 入力関連の毎フレーム処理
	//	input->Update();
	//	// 音声アップデート
	//	audio->Update();

	//	GlobalVariables::GetInstance()->Update();
	//	// ゲームシーンの毎フレーム処理
	//	gameScene->Update();
	//	// ImGui受付終了
	//	imguiManager->End();
	//	// 描画開始
	//	dxCommon->PreDraw();
	//	// ゲームシーンの描画
	//	gameScene->Draw();
	//	// ImGui描画
	//	imguiManager->Draw();
	//	// 描画終わり
	//	dxCommon->PostDraw();
	//}
		

	//// ゲームシーン解放
	//gameScene->Release();
	//SafeDelete(gameScene);

	ParticleManager::GetInstance()->Shutdown();

	// モデル
	ModelManager::GetInstance()->Shutdown();

	// スプライト
	Sprite::Release();

	// ImGui解放
	imguiManager->Finalize();

	// Line解放
	PrimitiveDrawer::Release();

	// テクスチャマネージャーの解放
	TextureManager::Release();

	// DirectXの解放
	dxCommon->Release();

	// ゲームウィンドウの破棄
	win->TerminateGameWindow();

	//// リリースリークチェック
	//IDXGIDebug1* debug;
	//if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
	//	debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
	//	debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
	//	debug->Release();
	//}
	return 0;
}

