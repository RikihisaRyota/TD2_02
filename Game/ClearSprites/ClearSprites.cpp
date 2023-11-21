#include "ClearSprites.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/StageData/StageData.h"
#include "MapChip.h"
#include "SceneSystem/IScene/IScene.h"

ClearSprites::ClearSprites() {
	input_ = Input::GetInstance();

	uint32_t tex = TextureManager::Load("Resources/Textures/white1x1.png");
	sprites_[SpriteNames::kBackground].reset(Sprite::Create(tex, Vector2{}));
	sprites_[SpriteNames::kBackground]->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	sprites_[SpriteNames::kBackground]->SetSize({ 1280.0f,720.0f });

	tex = TextureManager::Load("Resources/Textures/result.png");
	sprites_[SpriteNames::kResult].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/time.png");
	sprites_[SpriteNames::kTime].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/time0.png");
	sprites_[SpriteNames::kTimeOnesPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeTensPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeHundredsPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/starGet.png");
	sprites_[SpriteNames::kStarFirst].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kStarSecond].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kStarThird].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/result180s.png");
	sprites_[SpriteNames::kConditionFirst].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/result150s.png");
	sprites_[SpriteNames::kConditionSecond].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/result90s.png");
	sprites_[SpriteNames::kConditionThird].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/goStageSelect.png");
	sprites_[SpriteNames::kSelectStage].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/goNextStage.png");
	sprites_[SpriteNames::kNextStage].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/decisionA.png");
	sprites_[SpriteNames::kDecisionA].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/selectLS.png");
	sprites_[SpriteNames::kSelectLS].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/rank.png");
	sprites_[SpriteNames::kRank].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	for (uint32_t i = 0; i < kNumberCount; i++) {
		number_[i] = TextureManager::Load("Resources/Textures/time" + std::to_string(i) + ".png");
	}

	star_[kTrue] = TextureManager::Load("Resources/Textures/starGet.png");
	star_[kFalse] = TextureManager::Load("Resources/Textures/starNotGet.png");

	nextStage_[kTrue] = TextureManager::Load("Resources/Textures/goNextStageChoice.png");
	nextStage_[kFalse] = TextureManager::Load("Resources/Textures/goNextStage.png");

	selectStage_[kTrue] = TextureManager::Load("Resources/Textures/goStageSelectChoice.png");
	selectStage_[kFalse] = TextureManager::Load("Resources/Textures/goStageSelect.png");

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			if (j == V2ItemNames::kScale) {
				v2Info_[i][j] = sprites_[i]->GetSize();
			}
			else {
				v2Info_[i][j] = {};
			}
		}
	}

}

void ClearSprites::Init() {
	SetGlobalVariable();
	if (IScene::stageNo_ != MapChip::kCount - 1) {
		nextStageFlag_ = true;
	}
	else {
		nextStageFlag_ = false;
	}
	// 秒に直す
	int time = StageData::GetClearTime(IScene::stageNo_) / 60;
	int place = 100;
	if (time >= 999) {
		for (int i = 0; i < 3; i++) {
			timePlace_[i] = 9;
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
			timePlace_[2 - i] = time / place;
			time %= place;
			place /= 10;
		}
	}
	// 条件1
	if (StageData::GetClearFlag(IScene::stageNo_)) {
		starFlag_[0] = true;
	}
	else {
		starFlag_[0] = false;
	}
	// 条件2
	if (StageData::GetClearTime(IScene::stageNo_) <= StageData::GetConditionTime(IScene::stageNo_)) {
		starFlag_[1] = true;
	}
	else {
		starFlag_[1] = false;
	}
	const int clearItem = 3;
	// 条件3
	if (StageData::GetClearItemCount(IScene::stageNo_) >= StageData::GetConditionItemCount(IScene::stageNo_)) {
		starFlag_[2] = true;
	}
	else {
		starFlag_[2] = false;
	}
}

void ClearSprites::Update() {

	if (IScene::stageNo_ != MapChip::kCount - 1 &&
		input_->GetGamePadLStick().x != 0.0f &&
		input_->GetPreGamePadLStick().x == 0.0f) {
		nextStageFlag_ ^= true;
	}
	if (nextStageFlag_) {

	}
	else {

	}
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG
}

void ClearSprites::Draw() {
	for (int i = 0; auto & sprite : sprites_) {
		switch (i) {
		case ClearSprites::kTimeOnesPlace:
			sprite->SetTextureHandle(number_[timePlace_[0]]);
			sprite->Draw();
			break;
		case ClearSprites::kTimeTensPlace:
			sprite->SetTextureHandle(number_[timePlace_[1]]);
			sprite->Draw();
			break;
		case ClearSprites::kTimeHundredsPlace:
			sprite->SetTextureHandle(number_[timePlace_[2]]);
			sprite->Draw();
			break;
		case ClearSprites::kStarFirst:
			if (starFlag_[0]) {
				sprite->SetTextureHandle(star_[kTrue]);
			}
			else {
				sprite->SetTextureHandle(star_[kFalse]);
			}
			sprite->Draw();
			break;
		case ClearSprites::kStarSecond:
			if (starFlag_[1]) {
				sprite->SetTextureHandle(star_[kTrue]);
			}
			else {
				sprite->SetTextureHandle(star_[kFalse]);
			}
			sprite->Draw();
			break;
		case ClearSprites::kStarThird:
			if (starFlag_[2]) {
				sprite->SetTextureHandle(star_[kTrue]);
			}
			else {
				sprite->SetTextureHandle(star_[kFalse]);
			}
			sprite->Draw();
			break;
		case ClearSprites::kSelectStage:
			if (IScene::stageNo_ == MapChip::kCount - 1) {
				sprite->SetTextureHandle(selectStage_[kTrue]);
				sprite->SetPosition(Vector2(640.0f, 600.0f));
			}
			else {
				if (nextStageFlag_) {
					sprite->SetTextureHandle(selectStage_[kFalse]);
				}
				else {
					sprite->SetTextureHandle(selectStage_[kTrue]);
				}
				sprite->SetPosition(Vector2(450.0f, 600.0f));
			}
			sprite->Draw();
			break;
		case ClearSprites::kNextStage:
			if (IScene::stageNo_ != MapChip::kCount - 1) {
				if (nextStageFlag_) {
					sprite->SetTextureHandle(nextStage_[kTrue]);
				}
				else {
					sprite->SetTextureHandle(nextStage_[kFalse]);
				}
				sprite->Draw();

			}
			break;
		case ClearSprites::kBackground:
		case ClearSprites::kResult:
		case ClearSprites::kTime:
		case ClearSprites::kConditionFirst:
		case ClearSprites::kConditionSecond:
		case ClearSprites::kConditionThird:
		default:
			sprite->Draw();
			break;
		}
		i++;
	}
}
void ClearSprites::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void ClearSprites::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
		sprites_[i]->SetSize(v2Info_[i][V2ItemNames::kScale]);
	}
}
