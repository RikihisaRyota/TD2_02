#include "StageBanner.h"

#include "Ease/Ease.h"
#include "TextureManager.h"
#include "MyMath.h"
#include "SceneSystem/IScene/IScene.h"

#include "Input.h"
#include "ImGuiManager.h"

StageBanner::StageBanner() {
	stageTextureHandle_[MapChip::Stage::kStage_1] = TextureManager::Load("Resources/Textures/startStage1.png");
	stageTextureHandle_[MapChip::Stage::kStage_2] = TextureManager::Load("Resources/Textures/startStage2.png");
	stageTextureHandle_[MapChip::Stage::kStage_3] = TextureManager::Load("Resources/Textures/startStage3.png");
	stageTextureHandle_[MapChip::Stage::kStage_4] = TextureManager::Load("Resources/Textures/startStage4.png");
	stageTextureHandle_[MapChip::Stage::kStage_5] = TextureManager::Load("Resources/Textures/startStage5.png");
	stageTextureHandle_[MapChip::Stage::kStage_6] = TextureManager::Load("Resources/Textures/startStage6.png");
	stageTextureHandle_[MapChip::Stage::kStage_7] = TextureManager::Load("Resources/Textures/startStage7.png");
	stageTextureHandle_[MapChip::Stage::kStage_8] = TextureManager::Load("Resources/Textures/startStage8.png");
	stageTextureHandle_[MapChip::Stage::kStage_9] = TextureManager::Load("Resources/Textures/startStage9.png");
	stageTextureHandle_[MapChip::Stage::kStage_10] = TextureManager::Load("Resources/Textures/startStage10.png");
	stageTextureHandle_[MapChip::Stage::kStage_11] = TextureManager::Load("Resources/Textures/startStage11.png");
	stageTextureHandle_[MapChip::Stage::kStage_12] = TextureManager::Load("Resources/Textures/startStage12.png");
	stageTextureHandle_[MapChip::Stage::kStage_13] = TextureManager::Load("Resources/Textures/startStage13.png");
	stageTextureHandle_[MapChip::Stage::kStage_14] = TextureManager::Load("Resources/Textures/startStage14.png");
	stageTextureHandle_[MapChip::Stage::kStage_15] = TextureManager::Load("Resources/Textures/startStage15.png");
	stageTextureHandle_[MapChip::Stage::kStage_16] = TextureManager::Load("Resources/Textures/startStageEX.png");

	frameTextureHandle_ = TextureManager::Load("Resources/Textures/startFram.png");
	pressAndHoldTextureHandle_ = TextureManager::Load("Resources/Textures/longButton.png");

	startPosition_ = { 2560.0f,360.0f };
	endPosition_ = { -1280.0f,360.0f };
	kMaxAnimationCount_ = 120.0f;
	stageSprite_.reset(Sprite::Create(stageTextureHandle_[MapChip::Stage::kStage_1], startPosition_, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	frameSprite_.reset(Sprite::Create(frameTextureHandle_, startPosition_, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	pressAndHoldSprite_.reset(Sprite::Create(pressAndHoldTextureHandle_, startPosition_, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	stageSize_ = stageSprite_->GetSize();
	frameSize_ = frameSprite_->GetSize();
	pressAndHoldSize_ = pressAndHoldSprite_->GetSize();
	frameSize_.x *= 1.5f;
	frameSize_.y *= 0.5f;
	frameSprite_->SetSize(frameSize_);
	stagePosition_ = { 0.0f,0.0f };
	pressAndHoldPosition_ = { 0.0f,390.0f };
}

void StageBanner::Initialize() {
	isAnimation_ = true;
	animationCount_ = 0.0f;
	stageSprite_->SetTextureHandle(stageTextureHandle_[IScene::stageNo_]);
	/*if (IScene::stageNo_ == MapChip::Stage::kStage_4 ||
		IScene::stageNo_ == MapChip::Stage::kStage_9 ||
		IScene::stageNo_ == MapChip::Stage::kStage_13) {
		stagePosition_ = { 0.0f,334.0f };
	}
	else {*/
	stagePosition_ = { 0.0f,360.0f };
	/*}*/
}

void StageBanner::Update() {
	if (isAnimation_) {
		animationCount_ += 1.0f;
		float t = (animationCount_ / kMaxAnimationCount_);
		t = 4.0f * std::pow((t - 0.5f), 3.0f) + 0.5f;
		position_ = Lerp(startPosition_, endPosition_, t);

		stageSprite_->SetPosition({ position_.x,stagePosition_.y });
		frameSprite_->SetPosition(position_);
		pressAndHoldSprite_->SetPosition({ position_.x,pressAndHoldPosition_.y });
		if (animationCount_ >= kMaxAnimationCount_) {
			isAnimation_ = false;
		}
	}
	/*if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		animationCount_ = 0.0f;
		isAnimation_ = true;
	}*/
	//ImGui::Begin("Debug");
	//if (ImGui::TreeNode("Banner")) {
	//	ImGui::DragFloat2("start", &startPosition_.x, 1.0f);
	//	ImGui::DragFloat2("end", &endPosition_.x, 1.0f);
	//	ImGui::DragFloat2("stagePosition_", &stagePosition_.x, 1.0f);
	//	ImGui::DragFloat2("pressAndHoldPosition_", &pressAndHoldPosition_.x, 1.0f);
	//	ImGui::DragFloat2("stageSize", &stageSize_.x, 1.0f);
	//	ImGui::DragFloat2("frameSize", &frameSize_.x, 1.0f);
	//	ImGui::DragFloat2("pressAndHoldSize", &pressAndHoldSize_.x, 1.0f);
	//	/*stageSprite_->SetPosition(stagePosition_);
	//	frameSprite_->SetPosition(position_);
	//	pressAndHoldSprite_->SetPosition(pressAndHoldPosition_);*/
	//	ImGui::TreePop();
	//}
	//ImGui::End();
}

void StageBanner::Draw() {
	frameSprite_->Draw();
	stageSprite_->Draw();
	/*if (IScene::stageNo_ == MapChip::Stage::kStage_4 ||
		IScene::stageNo_ == MapChip::Stage::kStage_9 ||
		IScene::stageNo_ == MapChip::Stage::kStage_13) {
		pressAndHoldSprite_->Draw();
	}*/
}
