#include "StageBanner.h"

#include "Ease/Ease.h"
#include "TextureManager.h"
#include "MyMath.h"
#include "SceneSystem/IScene/IScene.h"

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
	frameSize_.y *= 0.5f;
	startPosition_ = { 1920.0f,360.0f };
	endPosition_ = { -640.0f,360.0f };
	kMaxAnimationCount_ = 120.0f;
	stageSprite_.reset(Sprite::Create(stageTextureHandle_[MapChip::Stage::kStage_1], startPosition_, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	frameSprite_.reset(Sprite::Create(frameTextureHandle_, startPosition_, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	stageSize_ = stageSprite_->GetSize();
	frameSize_ = frameSprite_->GetSize();
	frameSize_.y *= 0.5f;
	frameSprite_->SetSize(frameSize_);
}

void StageBanner::Initialize() {
	isAnimation_ = true;
	animationCount_ = 0.0f;
	stageSprite_->SetTextureHandle(stageTextureHandle_[IScene::stageNo_]);
}

void StageBanner::Update() {
	if (isAnimation_) {
		animationCount_ += 1.0f;
		float t = (animationCount_ / kMaxAnimationCount_);
		t = 4.0f * std::pow((t - 0.5f), 3.0f) + 0.5f;
		position_ = Lerp(startPosition_, endPosition_, t);

		stageSprite_->SetPosition(position_);
		frameSprite_->SetPosition(position_);
		if (animationCount_ >= kMaxAnimationCount_) {
			isAnimation_ = false;
		}
	}
}

void StageBanner::Draw() {
	frameSprite_->Draw();
	stageSprite_->Draw();
}
