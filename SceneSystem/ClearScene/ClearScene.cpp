#include "ClearScene.h"
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

ClearScene::ClearScene()
{
	// カメラの初期化
	viewProjection_.Initialize();

	clearSprites_ = std::make_unique<ClearSprites>();
}

void ClearScene::Init()
{
	clearSprites_->Init();
}

void ClearScene::Update()
{
	clearSprites_->Update();
	// フラグ
	if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
		sceneNo_ = SELECT;
	}
}

void ClearScene::Draw()
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
	clearSprites_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
#pragma endregion
}

void ClearScene::UIDraw() {
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
