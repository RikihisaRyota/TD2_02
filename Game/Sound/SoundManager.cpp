#include "SoundManager.h"


SoundManager::SoundManager() {
	audio_ = Audio::GetInstance();
	for (int32_t i = 0; i < Scene::kCount; i++) {
		playHandle_.push_back(int32_t(-1));
	}

	// タイトル
	soundHandle_.emplace_back(audio_->SoundLoadWave("BGM/title.wav"));
	// セレクト
	soundHandle_.emplace_back(audio_->SoundLoadWave("BGM/play.wav"));
	// ステージ
	soundHandle_.emplace_back(audio_->SoundLoadWave("BGM/play.wav"));
	// クリア
	soundHandle_.emplace_back(audio_->SoundLoadWave("BGM/title.wav"));

	playHandle_.at(kTitleScene) = audio_->SoundPlayLoopStart(soundHandle_.at(kTitleScene));
	preScene_ = 0;
	currentScene_ = 0;

	isLoop_ = true;
}

void SoundManager::Initialize(int currentScene, int preScene) {
	preScene_ = preScene;
	currentScene_ = currentScene;

	audio_->SoundPlayLoopEnd(playHandle_.at(preScene_));
	playHandle_.at(preScene_) = -1;

	isLoop_ = false;
}

void SoundManager::Update() {
	if (!isLoop_) {
		playHandle_.at(currentScene_) = audio_->SoundPlayLoopStart(soundHandle_.at(currentScene_));
		audio_->SetValume(playHandle_.at(currentScene_), 0.5f);
		isLoop_ = true;
	}
}
