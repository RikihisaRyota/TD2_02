#include "SceneChange.h"

#include "DirectXCommon.h"
#include "Ease/Ease.h"
#include "TextureManager.h"

SceneChange::SceneChange() {
	startColor_ = { 0.0f,0.0f,0.0f,0.0f };
	endColor_ = { 0.0f,0.0f,0.0f,1.0f };
	color_ = startColor_;
	background_.reset(Sprite::Create(0, { 0.0f,0.0f }, color_, { 0.5f,0.5f }));
	background_->SetSize({1280.0f*2.0f,720.0f*2.0f});
	time_ = 0.0f;
	maxTime_ = 60.0f;
	canInitialize_ = false;
	isSceneChange_ = false;
}

void SceneChange::Initialize() {
	time_ = 0.0f;
	StateRequest(State::kIn);
}

void SceneChange::Update() {
	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		preState_ = state_;
		stateRequest_ = std::nullopt;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();
}

void SceneChange::Draw() {
	Sprite::PreDraw(DirectXCommon::GetInstance()->GetCommandList());
	background_->Draw();
	Sprite::PostDraw();
}

void SceneChange::SetSceneChange(bool flag) {
	isSceneChange_ = true;
	StateRequest(kIn);
}

void SceneChange::StateRequest(State state) {
	if (state_ != state) {
		stateRequest_ = state;
	}
}

void SceneChange::NoneInitialize() {
	isSceneChange_ = false;
	canInitialize_ = false;
}

void SceneChange::NoneUpdate() {}

void SceneChange::InInitialize() {
	time_ = 0.0f;
	startColor_ = { 0.0f,0.0f,0.0f,0.0f };
	endColor_ = { 0.0f,0.0f,0.0f,1.0f };
	color_ = startColor_;
}

void SceneChange::InUpdate() {
	if (time_>=maxTime_) {
		StateRequest(kOut);
	}
	color_ = Ease::UseEase(startColor_, endColor_, time_, maxTime_, Ease::EaseType::EaseInOut);
	background_->SetColor(color_);
	time_ += 1.0f;
}

void SceneChange::OutInitialize() {
	time_ = 0.0f;
	startColor_ = { 0.0f,0.0f,0.0f,1.0f };
	endColor_ = { 0.0f,0.0f,0.0f,0.0f };
	color_ = startColor_;
	canInitialize_ = true;
}

void SceneChange::OutUpdate() {
	if (time_ >= maxTime_) {
		StateRequest(kNone);
	}
	color_ = Ease::UseEase(startColor_, endColor_, time_, maxTime_, Ease::EaseType::EaseInOut);
	background_->SetColor(color_);
	time_ += 1.0f;
}


void (SceneChange::* SceneChange::spStateInitFuncTable[])() {
	&SceneChange::NoneInitialize,
	&SceneChange::InInitialize,
	&SceneChange::OutInitialize,

};

void (SceneChange::* SceneChange::spStateUpdateFuncTable[])() {
	&SceneChange::NoneUpdate,
	&SceneChange::InUpdate,
	&SceneChange::OutUpdate,
};