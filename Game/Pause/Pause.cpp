#include "Pause.h"

#include "Audio.h"
#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"
#include "MyMath.h"

Pause::Pause() {
	uint32_t tex = TextureManager::Load("Resources/Textures/retryUi.png");
	sprites_[SpriteNames::kRetry].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/playStageSelect.png");
	sprites_[SpriteNames::kStageSelect].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	background_.reset(Sprite::Create(0, Vector2{}, { 0.0f,0.0f,0.0f,0.0f }, { 0.5f,0.5f }));
	background_->SetSize({ 1280.0f * 2.0f,720.0f * 2.0f });
	
	cancelSoundHandle_ = Audio::GetInstance()->SoundLoadWave("SE/cancel.wav");
	/*for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = {};
		}
	}*/
}

void Pause::Init() {
	SetGlobalVariable();
	isRetry_ = false;
	isSelect_ = false;
	isInitialize_ = false;
	isTransition_ = false;
	time_ = 0.0f;
	kMaxTime_ = 30.0f;
}

void Pause::Update() {
	ApplyGlobalVariable();

	if (isClear_) {
		if (!*isClear_) {
			if (!isTransition_) {
				if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::START)) {
					isTransition_ = true;
					auto playHandle = Audio::GetInstance()->SoundPlayWave(cancelSoundHandle_);
					Audio::GetInstance()->SetValume(playHandle, 0.5f);
				}
				else if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::BACK)) {
					isSelect_ = true;
					auto playHandle = Audio::GetInstance()->SoundPlayWave(cancelSoundHandle_);
					Audio::GetInstance()->SetValume(playHandle,0.5f);
				}
			}
			else {
				time_ += 1.0f;
				float t = time_ / kMaxTime_;
				if (t >= 1.0f) {
					separation_++;
					time_ = 0.0f;
				}
				if (separation_ <= 0) {
					background_->SetColor(Lerp(Vector4(0.0f, 0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f, 1.0f), time_ / kMaxTime_));
				}
				else if (separation_ <= 1) {
					isInitialize_ = true;
					background_->SetColor(Lerp(Vector4(0.0f, 0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 0.0f, 0.0f), time_ / kMaxTime_));
				}
				
				if (separation_ >= 2) {
					isRetry_ = true;
					isTransition_ = false;
					isInitialize_ = false;
					time_ = 0.0f;
					separation_ = 0;
				}
			}
		}
	}
}

void Pause::Draw() {
	//sprites_[SpriteNames::kRetry]->Draw();
	for (const std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
	
}

void Pause::DrawBackground() {
	background_->Draw();
}

void Pause::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void Pause::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
	}
}
