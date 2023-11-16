#include "GameScene.h"

#include "DirectXCommon.h"
#include "MyMath.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "Collision/CollisionManager.h"

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
	mapChip_->Initialize();
	mapChip_->SetViewProjection(&viewProjection_);
	mapChip_->LoadCSV("stage_1");
	mapChipEditor_->SetMapChip(mapChip_.get());
	mapChipEditor_->SetViewProjection(&viewProjection_);
	mapChipEditor_->Initialize();
#pragma endregion

	followCamera_ = std::make_unique<FollowCamera>();

	player_ = std::make_unique<Player>();
	player_->Initialize();

	followCamera_->SetTarget(player_->GetWorldTransform());
	followCamera_->Initialize();
}

void GameScene::Update() {
	CollisionManager* collisionManager = CollisionManager::GetInstance();
	collisionManager->Clear();

	if (input_->TriggerKey(DIK_TAB)) {
		isDebug_ ^= true;
		if (isDebug_) {
			int32_t x =int32_t(MakeTranslate(player_->GetWorldTransform()->matWorld_).x / float(kBlockSize));
			x = std::clamp(x, 16,int32_t(kMaxWidthBlockNum));
			int32_t y =int32_t(MakeTranslate(player_->GetWorldTransform()->matWorld_).y / float(kBlockSize));
			y = std::clamp(y, 10,int32_t(kMaxHeightBlockNum));
			viewProjection_.translate_.x = float(x	)* float(kBlockSize);
			viewProjection_.translate_.y = float(y)* float(kBlockSize);
			viewProjection_.translate_.z = viewProjection_.kInitializeTranslate_.z;
			viewProjection_.UpdateMatrix();
		}

	}

	mapChip_->Update();

	player_->Update();

	collisionManager->CheckCollision();


	if (!isDebug_) {
		followCamera_->Update();
		viewProjection_ = followCamera_->GetViewProjection();
	}
	else {
		// マップチップエディター
		mapChipEditor_->Update();
	}
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
	player_->Draw(viewProjection_);

	mapChipEditor_->Draw();
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
