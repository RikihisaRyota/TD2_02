#include "Timer.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"
#include "MapChip.h"
#include <numbers>
#include "TextureManager.h"
#include "SceneSystem/IScene/IScene.h"
#include "Game/StageData/StageData.h"

Timer::Timer()
{
	uint32_t tex = TextureManager::Load("Resources/Textures/time.png");
	sprites_[SpriteNames::kTimerSprite].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	timerSize_ = sprites_[SpriteNames::kTimerSprite]->GetSize();

	numTeces_[TexColor::kBright][0] = TextureManager::Load("Resources/Textures/resultTime0.png");
	numTeces_[TexColor::kBright][1] = TextureManager::Load("Resources/Textures/resultTime1.png");
	numTeces_[TexColor::kBright][2] = TextureManager::Load("Resources/Textures/resultTime2.png");
	numTeces_[TexColor::kBright][3] = TextureManager::Load("Resources/Textures/resultTime3.png");
	numTeces_[TexColor::kBright][4] = TextureManager::Load("Resources/Textures/resultTime4.png");
	numTeces_[TexColor::kBright][5] = TextureManager::Load("Resources/Textures/resultTime5.png");
	numTeces_[TexColor::kBright][6] = TextureManager::Load("Resources/Textures/resultTime6.png");
	numTeces_[TexColor::kBright][7] = TextureManager::Load("Resources/Textures/resultTime7.png");
	numTeces_[TexColor::kBright][8] = TextureManager::Load("Resources/Textures/resultTime8.png");
	numTeces_[TexColor::kBright][9] = TextureManager::Load("Resources/Textures/resultTime9.png");

	numTeces_[TexColor::kDark][0] = TextureManager::Load("Resources/Textures/time0.png");
	numTeces_[TexColor::kDark][1] = TextureManager::Load("Resources/Textures/time1.png");
	numTeces_[TexColor::kDark][2] = TextureManager::Load("Resources/Textures/time2.png");
	numTeces_[TexColor::kDark][3] = TextureManager::Load("Resources/Textures/time3.png");
	numTeces_[TexColor::kDark][4] = TextureManager::Load("Resources/Textures/time4.png");
	numTeces_[TexColor::kDark][5] = TextureManager::Load("Resources/Textures/time5.png");
	numTeces_[TexColor::kDark][6] = TextureManager::Load("Resources/Textures/time6.png");
	numTeces_[TexColor::kDark][7] = TextureManager::Load("Resources/Textures/time7.png");
	numTeces_[TexColor::kDark][8] = TextureManager::Load("Resources/Textures/time8.png");
	numTeces_[TexColor::kDark][9] = TextureManager::Load("Resources/Textures/time9.png");

	for (std::array<std::unique_ptr<Sprite>, MaxDigits>& spriteArray : numSprites_) {
		for (std::unique_ptr<Sprite>& sprite : spriteArray) {
			sprite.reset(Sprite::Create(numTeces_[TexColor::kBright][0], Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
		}
	}

	numSize_ = numSprites_[0][0]->GetSize();

	Init();
	SetGlobalVariable();
}

void Timer::Init()
{
	time_ = 0;
	if (isClear_) {
		second_ = 0;
	}
	else if (stage_) {
		second_ = StageData::GetConditionTime(*stage_) / 60;
	}
	SetSpriteSize();
	SetNumTeces();
}


void Timer::SetNumTeces()
{
	int num = second_;
	int drawNum = 0;
	for (int i = 0; i < MaxDigits; i++) {
		drawNum = num / int(pow(10, MaxDigits - 1 - i));
		num = num % int(pow(10, MaxDigits - 1 - i));

		if (isClear_) {

			if (second_ <= StageData::GetConditionTime(IScene::stageNo_)) {
				numSprites_[DrawNumType::kTimer][i]->SetTextureHandle(numTeces_[TexColor::kBright][drawNum]);
			}
			else {
				numSprites_[DrawNumType::kTimer][i]->SetTextureHandle(numTeces_[TexColor::kDark][drawNum]);
			}

		}
		else {
			numSprites_[DrawNumType::kTimer][i]->SetTextureHandle(numTeces_[TexColor::kBright][drawNum]);
		}
	}
}

void Timer::SetSpriteSize()
{
	sprites_[SpriteNames::kTimerSprite]->SetSize(timerSize_ * fInfo_[FInfoNames::kTimerScale]);

	for (std::array<std::unique_ptr<Sprite>, MaxDigits>& spriteArray : numSprites_) {
		for (std::unique_ptr<Sprite>& sprite : spriteArray) {
			sprite->SetSize(numSize_ * fInfo_[FInfoNames::kNumScale]);
		}
	}
}

void Timer::Update()
{
	ApplyGlobalVariable();

	if (isClear_) {
		if (!*isClear_) {

			time_++;

			second_ = time_ / 60;

			if (second_ >= 999) {
				second_ = 999;
			}
			else if (second_ < 0) {
				second_ = 0;
			}

			SetNumTeces();
		}
	}
	else if (stage_) {

		second_ = StageData::GetConditionTime(*stage_) / 60;

		if (second_ >= 999) {
			second_ = 999;
		}
		else if (second_ < 0) {
			second_ = 0;
		}

		SetNumTeces();
	}

}

void Timer::DrawUI()
{
	for (const std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}

	for (const std::array<std::unique_ptr<Sprite>, MaxDigits>& spriteArray : numSprites_) {
		for (const std::unique_ptr<Sprite>& sprite : spriteArray) {
			sprite->Draw();
		}
	}
}

void Timer::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	for (int i = 0; i < DrawNumType::kNumTypeCount; i++) {
		globalVariables->AddItem(groupName_, numItemNames[i] + v2ItemNames_[V2ItemNames::kPos], numPoses_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	/*for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}*/

	globalVariables->CreateGroup(groupName2_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName2_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	for (int i = 0; i < DrawNumType::kNumTypeCount; i++) {
		globalVariables->AddItem(groupName2_, numItemNames[i] + v2ItemNames_[V2ItemNames::kPos], numPoses_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables->AddItem(groupName2_, fInfoNames_[i], fInfo_[i]);
	}

	ApplyGlobalVariable();
}

void Timer::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	if (stage_) {
		for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
			for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
				v2Info_[i][j] = globalVariables->GetVector2Value(groupName2_, spriteNames_[i] + v2ItemNames_[j]);
			}

			sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
		}

		for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
			fInfo_[i] = globalVariables->GetFloatValue(groupName2_, fInfoNames_[i]);
		}

		for (int i = 0; i < DrawNumType::kNumTypeCount; i++) {
			numPoses_[i] = globalVariables->GetVector2Value(groupName2_, numItemNames[i] + v2ItemNames_[V2ItemNames::kPos]);

			numSprites_[i][0]->SetPosition({ numPoses_[i].x - fInfo_[FInfoNames::kNumericInterval],numPoses_[i].y });
			numSprites_[i][1]->SetPosition(numPoses_[i]);
			numSprites_[i][2]->SetPosition({ numPoses_[i].x + fInfo_[FInfoNames::kNumericInterval],numPoses_[i].y });
		}
	}
	else {
		for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
			for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
				v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
			}

			sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
		}

		for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
			fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
		}

		for (int i = 0; i < DrawNumType::kNumTypeCount; i++) {
			numPoses_[i] = globalVariables->GetVector2Value(groupName_, numItemNames[i] + v2ItemNames_[V2ItemNames::kPos]);

			numSprites_[i][0]->SetPosition({ numPoses_[i].x - fInfo_[FInfoNames::kNumericInterval],numPoses_[i].y });
			numSprites_[i][1]->SetPosition(numPoses_[i]);
			numSprites_[i][2]->SetPosition({ numPoses_[i].x + fInfo_[FInfoNames::kNumericInterval],numPoses_[i].y });
		}
	}

	/*for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}*/

	SetSpriteSize();
}
