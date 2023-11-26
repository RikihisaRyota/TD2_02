#include "StageUI.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"
#include "SceneSystem/IScene/IScene.h"

StageUI::StageUI() {
	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		models_[i] = std::make_unique<PlaneRenderer>();
		models_[i].reset(PlaneRenderer::Create());

		worldTransforms_[i].Initialize();
		isDraws_[i] = false;
	}

	teces_[SpriteNames::kJumpUi] = TextureManager::Load("Resources/Textures/jumpUi.png");
	teces_[SpriteNames::kLStickLeft] = TextureManager::Load("Resources/Textures/LstickLeft.png");
	teces_[SpriteNames::kLStickRight] = TextureManager::Load("Resources/Textures/LstickRight.png");
	teces_[SpriteNames::kWallJumpLeft] = TextureManager::Load("Resources/Textures/wallJumpLeft.png");
	teces_[SpriteNames::kWallJumpRight] = TextureManager::Load("Resources/Textures/wallJumpRight.png");

	SetGlobalVariable();
	SetPos();
}

void StageUI::Init() {
	
	if (IScene::stageNo_ == 0) {
		for (int i = 0; i < SpriteNames::kSpriteCount; i++) {

			if (i == SpriteNames::kJumpUi || i == SpriteNames::kLStickLeft) {
				isDraws_[i] = true;
			}
			else {
				isDraws_[i] = false;
			}
		}
	}
	else if (IScene::stageNo_ == 1) {
		for (int i = 0; i < SpriteNames::kSpriteCount; i++) {

			if (i == SpriteNames::kWallJumpLeft || i == SpriteNames::kWallJumpRight) {
				isDraws_[i] = true;
			}
			else {
				isDraws_[i] = false;
			}
		}
	}
	else {
		for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
			isDraws_[i] = false;
		}
	}

	SetPos();
	countFrame_ = 0;
}

void StageUI::Update() {

#ifdef _DEBUG
	ApplyGlobalVariable();
	SetPos();
#endif // _DEBUG

	if (IScene::stageNo_ == 0) {
		countFrame_++;

		if (countFrame_ == 30) {
			countFrame_ = 0;
			if (isDraws_[SpriteNames::kLStickLeft]) {
				isDraws_[SpriteNames::kLStickLeft] = false;
				isDraws_[SpriteNames::kLStickRight] = true;
			}
			else {
				isDraws_[SpriteNames::kLStickLeft] = true;
				isDraws_[SpriteNames::kLStickRight] = false;
			}
		}
	}

}

void StageUI::Draw(const ViewProjection& viewProjection)
{
	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		if (isDraws_[i]) {
			models_[i]->Draw(worldTransforms_[i], viewProjection, teces_[i]);
		}
	}
}

void StageUI::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	ApplyGlobalVariable();
}

void StageUI::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}
	}
}

void StageUI::SetPos()
{
	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		if (i == SpriteNames::kJumpUi || i == SpriteNames::kLStickLeft || i == SpriteNames::kLStickRight) {
			worldTransforms_[i].translate_ = { v2Info_[i][V2ItemNames::kPos].x,v2Info_[i][V2ItemNames::kPos].y,fInfo_[FInfoNames::kNearZ] };
		}
		else {
			worldTransforms_[i].translate_ = { v2Info_[i][V2ItemNames::kPos].x,v2Info_[i][V2ItemNames::kPos].y,fInfo_[FInfoNames::kNearZ] };
		}

		worldTransforms_[i].scale_.x = fInfo_[FInfoNames::kScale];
		worldTransforms_[i].scale_.y = fInfo_[FInfoNames::kScale];
		worldTransforms_[i].UpdateMatrix();
	}
}
