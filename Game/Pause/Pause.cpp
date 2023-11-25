#include "Pause.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"

Pause::Pause() {
	uint32_t tex = TextureManager::Load("Resources/Textures/retryUi.png");
	sprites_[SpriteNames::kRetry].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	/*for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = {};
		}
	}*/
}

void Pause::Init() {
	SetGlobalVariable();
	isRetry_ = false;
}

void Pause::Update() {
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG

	if (isClear_) {
		if (!*isClear_) {

			if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::START)) {
				isRetry_ = true;
			}
		}
	}
}

void Pause::Draw()
{
	sprites_[SpriteNames::kRetry]->Draw();
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
