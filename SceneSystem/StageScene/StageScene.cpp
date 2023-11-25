#include "StageScene.h"

#include "Audio.h"
#include "Input.h"
#include "PrimitiveDrawer.h"
#include "DirectXCommon.h"
#include "MyMath.h"
#include "Collision/CollisionManager.h"
#include "Sprite.h"
#include "PlaneRenderer.h"
#include "OBJ.h"
#include "CubeRenderer.h"

#include "ParticleManager.h"
#include "ImGuiManager.h"
#include "SceneSystem/IScene/IScene.h"
#include "Game/StageData/StageData.h"
#include "Game/Nedle/Needle.h"
#include "Game/Item/Item.h"

StageScene::StageScene()
{
#pragma region 
	
	// デバックカメラ
	debugCamera_ = std::make_unique<DebugCamera>();
	// カメラの初期化
	viewProjection_.Initialize();
	// 線
	PrimitiveDrawer::GetInstance()->SetViewProjection(&viewProjection_);
#pragma endregion
	isDebug_ = false;

#pragma region 生成
	NeedleManager::GetInstance()->FirstInit();
	ItemManager::GetInstance()->FirstInit();
	background_ = std::make_unique<Background>();
	followCamera_ = std::make_unique<FollowCamera>();
	goal_ = std::make_unique<Goal>();
	mapChip_ = std::make_unique<MapChip>();
	mapChipEditor_ = std::make_unique<MapChipEditor>();
	player_ = std::make_unique<Player>();
	pause_ = std::make_unique<Pause>();
#pragma endregion
	background_->SetPlayer(player_.get());

	mapChip_->Initialize(viewProjection_);
	mapChip_->SetViewProjection(&viewProjection_);

	mapChipEditor_->SetMapChip(mapChip_.get());
	mapChipEditor_->SetViewProjection(&viewProjection_);
	mapChipEditor_->Initialize();
	
	followCamera_->SetTarget(player_->GetWorldTransform());
	pause_->SetIsClear(player_->GetIsCollisionGaolPtr());
}

void StageScene::Init()
{
	NeedleManager::GetInstance()->Init();
	background_->Initialize();
	goal_->Initialize();
	player_->Initialize();
	followCamera_->SetTarget(player_->GetWorldTransform());
	followCamera_->Initialize();
	followCamera_->Update();
	viewProjection_ = followCamera_->GetViewProjection();
	mapChip_->SetCurrentStage(IScene::stageNo_);
	mapChip_->Initialize(viewProjection_);
	pause_->Init();
}

void StageScene::Update()
{
	// 入力と処理受付
	pause_->Update();
	
	if (pause_->GetIsRetry() || player_->GetIsDead()) {
		Init();
		return;
	}


	CollisionManager* collisionManager = CollisionManager::GetInstance();
	collisionManager->Clear();

	if (Input::GetInstance()->TriggerKey(DIK_TAB)) {
		isDebug_ ^= true;
		if (isDebug_) {
			int32_t x = int32_t(MakeTranslate(player_->GetWorldTransform()->matWorld_).x / float(kBlockSize));
			x = std::clamp(x, 16, int32_t(kMaxWidthBlockNum));
			int32_t y = int32_t(MakeTranslate(player_->GetWorldTransform()->matWorld_).y / float(kBlockSize));
			y = std::clamp(y, 10, int32_t(kMaxHeightBlockNum));
			viewProjection_.translate_.x = float(x) * float(kBlockSize);
			viewProjection_.translate_.y = float(y) * float(kBlockSize);
			viewProjection_.translate_.z = viewProjection_.kInitializeTranslate_.z;
			viewProjection_.UpdateMatrix();
		}
	}
	goal_->Update();

	background_->Update();

	mapChip_->Update(viewProjection_);

	NeedleManager::GetInstance()->Update();

	player_->Update();

	ItemManager::GetInstance()->Update();

	collisionManager->CheckCollision();

	if (player_->GetIsChangeCamera()) {
		followCamera_->SetTarget(goal_->GetWorldTransform());
		followCamera_->ChangeCamera();
	}

	if (!isDebug_) {
		followCamera_->Update();
		viewProjection_ = followCamera_->GetViewProjection();
	}
	else {
		// マップチップエディター
		mapChipEditor_->Update();
	}

	// クリアフラグ
	if (player_->GetIsClear() ||
		Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::X)) {
		StageData::SetData(player_->GetClearTime(), ItemManager::GetInstance()->GetClearItemCountNum(), ItemManager::GetInstance()->GetMaxItemNum(), true, IScene::stageNo_);
		sceneNo_ = CLEAR;
	}

	ParticleManager::GetInstance()->Update();
}

void StageScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	// スプライト描画後処理
	background_->SpriteDraw();
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
	background_->Draw(viewProjection_);
	goal_->Draw(viewProjection_);
	mapChip_->Draw(viewProjection_);
	player_->Draw(viewProjection_);
	NeedleManager::GetInstance()->Draw(viewProjection_);
	ItemManager::GetInstance()->Draw(viewProjection_);

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

void StageScene::UIDraw() {
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	pause_->Draw();

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

	ItemManager::GetInstance()->DrawUI();

	// スプライト描画後処理
	Sprite::PostDraw();
}
