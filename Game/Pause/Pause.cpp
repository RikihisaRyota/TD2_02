#include "Pause.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"

Pause::Pause() {
	uint32_t tex = TextureManager::Load("Resources/Textures/white1x1.png");
	sprites_[SpriteNames::kBackground].reset(Sprite::Create(tex, Vector2{}, { 0.0f,0.0f,0.0f,1.0f }, { 0.5f,0.5f }));
	sprites_[SpriteNames::kBackground]->SetSize({ 1280.0f,720.0f });

	tex = TextureManager::Load("Resources/Textures/title.png");
	sprites_[SpriteNames::kTitle].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/pushA.png");
	sprites_[SpriteNames::kDecisionA].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = {};
		}
	}

	testTextureHandle_[kOn] = TextureManager::Load("Resources/Textures/title.png");
	testTextureHandle_[kOff] = TextureManager::Load("Resources/Textures/titleOff.png");
}

void Pause::Init() {
	SetGlobalVariable();
}

void Pause::Update() {
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG

	if (isClear_) {
		if (!*isClear_) {




		}
	}
}

void Pause::Draw()
{
	sprites_[SpriteNames::kBackground]->Draw();
	sprites_[SpriteNames::kTitle]->Draw();
	sprites_[SpriteNames::kDecisionA]->Draw();
}

void Pause::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void Pause::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
	}
}
