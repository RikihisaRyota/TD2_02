#include "GameScene.h"

#include "DirectXCommon.h"
#include "MyMath.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

#include "ParticleManager.h"
#include "ParticleShaderStruct.h"

GameScene::GameScene() {}

GameScene::~GameScene() {}

void GameScene::Initialize() {
#pragma region 
	dxCommon_ = DirectXCommon::GetInstance();
	// デバックカメラ
	debugCamera_ = new DebugCamera();
	// 入力
	input_ = Input::GetInstance();
	// カメラの初期化
	viewProjection_.Initialize();
	// 線
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);
#pragma endregion
	isDebug_ = false;
#pragma region 生成
	mapChip_ = std::make_unique<MapChip>();
	mapChipEditor_ = std::make_unique<MapChipEditor>();
#pragma endregion

#pragma region 初期化
	mapChip_->LoadCSV("stage_1");
	mapChip_->SetViewProjection(&viewProjection_);
	mapChip_->Initialize();
	mapChipEditor_->SetMapChip(mapChip_.get());
	mapChipEditor_->SetViewProjection(&viewProjection_);
	mapChipEditor_->Initialize();
#pragma endregion
}

void GameScene::Update() {
	if (input_->TriggerKey(DIK_TAB)) {
		isDebug_ ^= true;
	}
	// マップチップエディター
	mapChipEditor_->Update();
	if (!isDebug_) {
		// デバックカメラ
		debugCamera_->Update(&viewProjection_);
	}
	if (input_->TriggerKey(DIK_SPACE)) {
  		Emitter* emitter = new Emitter();
		emitter->aliveTime = 1;
		emitter->aliveTime = 10;
		emitter->position = { 4.0f,4.0f,0.0f };
		emitter->isAlive = true;
		ParticleMotion* particleMotion = new ParticleMotion();
		particleMotion->angle.start = DegToRad(0.0f);
		particleMotion->angle.end= DegToRad(180.0f);
		particleMotion->color.currentColor= { 1.0f,1.0f,1.0f,1.0f };
		particleMotion->color.startColor = { 1.0f,1.0f,1.0f,1.0f };
		particleMotion->color.endColor = { 1.0f,1.0f,1.0f,0.0f };
		particleMotion->scale.currentScale = { 1.0f,1.0f,1.0f };
		particleMotion->scale.startScale= { 1.0f,1.0f,1.0f };
		particleMotion->scale.endScale= { 0.5f,0.5f,0.5f };
		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
		particleMotion->rotate.currentRotate= { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 5.0f;
		particleMotion->velocity.randomRange = 1.0f;
		particleMotion->aliveTime.time = 20;
		particleMotion->aliveTime.randomRange = 5;
		particleMotion->isAlive = true;
		ParticleManager::GetInstance()->AddParticle(emitter, particleMotion,0);
	}
	ParticleManager::GetInstance()->Update();
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	dxCommon_->ClearDepthBuffer();
#pragma endregion
#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	CubeRenderer::PreDraw(commandList);
	SphereRenderer::PreDraw(commandList);
	OBJ::PreDraw(commandList);
	Model::PreDraw(commandList);
	PrimitiveDrawer::PreDraw(commandList);
	PlaneRenderer::PreDraw(commandList);
	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	mapChip_->Draw(viewProjection_);

	mapChipEditor_->Draw();

	ParticleManager::GetInstance()->Draw(viewProjection_);

	PrimitiveDrawer::Draw();
	// 3Dオブジェクト描画後処理
	PlaneRenderer::PostDraw();
	PrimitiveDrawer::PostDraw();
	Model::PostDraw();
	SphereRenderer::PostDraw();
	OBJ::PostDraw();
	CubeRenderer::PostDraw();
#pragma endregion



#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	Sprite::SetBlendState(Sprite::BlendState::kNormal);

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion

}

void GameScene::Release() {
	SafeDelete(debugCamera_);
}
