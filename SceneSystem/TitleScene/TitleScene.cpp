#include "TitleScene.h"
#include "Input.h"
#include "PrimitiveDrawer.h"
#include "DirectXCommon.h"
#include "MyMath.h"
#include "Sprite.h"
#include "PlaneRenderer.h"
#include "OBJ.h"
#include "CubeRenderer.h"
#include "SphereRenderer.h"
#include "Model.h"

#include "Audio.h"
#include "MyMath.h"
#include "ParticleUIManager.h"

TitleScene::TitleScene() {
	// カメラの初期化
	viewProjection_.Initialize();
	titleSprites_ = std::make_unique<TitleSprites>();
	choiceSoundHandle_ = Audio::GetInstance()->SoundLoadWave("SE/choice.wav");
}

void TitleScene::Init() {
	titleSprites_->Init();
}

void TitleScene::Update() {

	titleSprites_->Update();
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Emitter* verticalEmitter = new Emitter();
		ParticleMotion* verticalParticleMotion = new ParticleMotion();

		verticalEmitter->aliveTime = 1;
		verticalEmitter->flameInterval = 0;
		verticalEmitter->spawn.position = { 0.0f,0.0f,0.0f };
		verticalEmitter->spawn.rangeX = 0.0f;
		verticalEmitter->spawn.rangeY = 0.0f;
		verticalEmitter->inOnce = 1;
		verticalEmitter->angle.start = DegToRad(0.0f);
		verticalEmitter->angle.end = DegToRad(360.0f);
		verticalEmitter->isAlive = true;

		verticalParticleMotion->color.startColor = { 1.0f,1.0f,1.0f,1.0f };
		verticalParticleMotion->color.endColor = { 1.0f,1.0f,1.0f,1.0f };
		verticalParticleMotion->color.currentColor = verticalParticleMotion->color.startColor;
		verticalParticleMotion->scale.startScale = { 1.0f,1.0f,1.0f };
		verticalParticleMotion->scale.interimScale = { 1.0f,1.0f,1.0f };
		verticalParticleMotion->scale.endScale = { 1.0f,1.0f,1.0f };
		verticalParticleMotion->scale.currentScale = verticalParticleMotion->scale.startScale;
		verticalParticleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
		verticalParticleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		verticalParticleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		verticalParticleMotion->velocity.speed = 1.0f;
		verticalParticleMotion->velocity.randomRange = 0.0f;
		verticalParticleMotion->acceleration_ = {};
		verticalParticleMotion->aliveTime.time = 30;
		verticalParticleMotion->aliveTime.randomRange = 0;
		verticalParticleMotion->isAlive = true;
		ParticleUIManager::GetInstance()->AddParticle(verticalEmitter, verticalParticleMotion, 0); 
	}
	// フラグ
	if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
		Audio::GetInstance()->SoundPlayWave(choiceSoundHandle_);
		sceneNo_ = SELECT;
	}
	ParticleUIManager::GetInstance()->Update();
}

void TitleScene::Draw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	titleSprites_->FarDraw();

	// スプライト描画後処理
	Sprite::PostDraw();
	DirectXCommon::GetInstance()->ClearDepthBuffer();
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
	titleSprites_->NearDraw();

	ParticleUIManager::GetInstance()->Draw();
	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void TitleScene::UIDraw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	DirectXCommon::GetInstance()->ClearDepthBuffer();
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
}
