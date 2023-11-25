#include "TitleSprites.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"
#include "ImGuiManager.h"

TitleSprites::TitleSprites() {
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

	flashingCount_ = rnd_.NextIntRange(onMin_, onMax_);
	on_ = true;
	onMin_ = 20;
	onMax_ = 300;
	offMin_ = 5;
	offMax_ = 15;
}

void TitleSprites::Init() {
	SetGlobalVariable();
}

void TitleSprites::Update() {
	flashingCount_--;
	if (flashingCount_ <= 0) {
		if (on_) {
			sprites_[SpriteNames::kTitle]->SetTextureHandle(testTextureHandle_[kOn]);
			flashingCount_ = rnd_.NextIntRange(onMin_, onMax_);
		}
		else {
			sprites_[SpriteNames::kTitle]->SetTextureHandle(testTextureHandle_[kOff]);
			flashingCount_ = rnd_.NextIntRange(offMin_, offMax_);
		}
		on_ ^= true;
	}
#ifdef _DEBUG
	ImGui::Begin("Debug");
	if (ImGui::TreeNode("Title")) {
		if (ImGui::TreeNode("On")) {
			ImGui::DragInt("onMin", &onMin_);
			ImGui::DragInt("onMax", &onMax_);
			ImGui::TreePop();
		}
		if (ImGui::TreeNode("Off")) {
			ImGui::DragInt("offMin", &offMin_);
			ImGui::DragInt("offMax", &offMax_);
			ImGui::TreePop();
		}
		ImGui::TreePop();
	}
	ImGui::End();
	ApplyGlobalVariable();
#endif // _DEBUG
}

void TitleSprites::FarDraw() {
	sprites_[SpriteNames::kBackground]->Draw();
}

void TitleSprites::NearDraw() {

	sprites_[SpriteNames::kTitle]->Draw();
	sprites_[SpriteNames::kDecisionA]->Draw();
}

void TitleSprites::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void TitleSprites::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
	}
}
