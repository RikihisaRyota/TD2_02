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

TimerManager* TimerManager::GetInstance()
{
	static TimerManager instance;
	return &instance;
}

void TimerManager::FirstInit()
{
	
	uint32_t tex = TextureManager::Load("Resources/Textures/item.png");
	sprites_[SpriteNames::kItemSprite].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	itemSize_ = sprites_[SpriteNames::kItemSprite]->GetSize();

	tex = TextureManager::Load("Resources/Textures/slash.png");
	sprites_[SpriteNames::kSlash].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	slashSize_ = sprites_[SpriteNames::kSlash]->GetSize();

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

void TimerManager::Init()
{
	SetSpriteSize();
	SetNumTeces();
	countFrame_ = 0;
}


void TimerManager::SetNumTeces()
{
	if (MaxItemCount_ < 10) {

		numSprites_[DrawNumType::kMaxItem][0]->SetTextureHandle(numTeces_[TexColor::kBright][0]);
		numSprites_[DrawNumType::kGetItem][0]->SetTextureHandle(numTeces_[TexColor::kBright][0]);

		numSprites_[DrawNumType::kMaxItem][1]->SetTextureHandle(numTeces_[TexColor::kBright][MaxItemCount_]);
		numSprites_[DrawNumType::kGetItem][1]->SetTextureHandle(numTeces_[TexColor::kBright][getItemCount_]);

		return;
	}
	else {

		int num = MaxItemCount_;
		int drawNum = 0;
		for (int i = 0; i < MaxDigits; i++) {
			drawNum = num / int(pow(10, MaxDigits - 1 - i));
			num = num % int(pow(10, MaxDigits - 1 - i));

			numSprites_[DrawNumType::kMaxItem][i]->SetTextureHandle(numTeces_[TexColor::kBright][drawNum]);
		}

		if (getItemCount_ < 10) {
			numSprites_[DrawNumType::kGetItem][0]->SetTextureHandle(numTeces_[TexColor::kBright][0]);
			numSprites_[DrawNumType::kGetItem][1]->SetTextureHandle(numTeces_[TexColor::kBright][getItemCount_]);
		}
		else {
			num = getItemCount_;
			for (int i = 0; i < MaxDigits; i++) {
				drawNum = num / int(pow(10, MaxDigits - 1 - i));
				num = num % int(pow(10, MaxDigits - 1 - i));

				numSprites_[DrawNumType::kGetItem][i]->SetTextureHandle(numTeces_[TexColor::kBright][drawNum]);
			}
		}
	}
}

void TimerManager::SetSpriteSize()
{
	sprites_[SpriteNames::kItemSprite]->SetSize(itemSize_ * fInfo_[FInfoNames::kItemScale]);
	sprites_[SpriteNames::kSlash]->SetSize(slashSize_ * fInfo_[FInfoNames::kSlashScale_]);

	for (std::array<std::unique_ptr<Sprite>, MaxDigits>& spriteArray : numSprites_) {
		for (std::unique_ptr<Sprite>& sprite : spriteArray) {
			sprite->SetSize(numSize_ * fInfo_[FInfoNames::kNumScale]);
		}
	}
}

void TimerManager::Update()
{
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG

	countFrame_++;



	/*for (const std::unique_ptr<Item>& item : items_) {
		item->Update();
	}*/
}

void TimerManager::Draw(const ViewProjection& viewProjection)
{
	
}

void TimerManager::DrawUI()
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

void TimerManager::SetGlobalVariable()
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

	ApplyGlobalVariable();
}

void TimerManager::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

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
		numSprites_[i][1]->SetPosition({ numPoses_[i].x + fInfo_[FInfoNames::kNumericInterval],numPoses_[i].y });
	}

	/*for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}*/

	SetSpriteSize();
}
