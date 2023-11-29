#include "SelectSprites.h"

#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "SceneSystem/IScene/IScene.h"
#include "Input.h"
#include "Ease/Ease.h"
#include "MapChip.h"
#include "Audio.h"
#include <numbers>

SelectSprites::SelectSprites() {
	uint32_t tex = TextureManager::Load("Resources/Textures/white1x1.png");
	blackBackground_.reset(Sprite::Create(tex, Vector2{ 640.0f,360.0f }, { 0.0f,0.0f,0.0f,1.0f }, { 0.5f,0.5f }));
	blackBackground_->SetSize({ 1280.0f,720.0f });

	tex = TextureManager::Load("Resources/Textures/stageSelect.png");
	sprites_[SpriteNames::kSelect].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/decisionA.png");
	sprites_[SpriteNames::kDecisionA].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/selectLS.png");
	sprites_[SpriteNames::kSelectLS].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/goTitle.png");
	sprites_[SpriteNames::kGoTitle].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/stageSelectFram.png");
	sprites_[SpriteNames::kStarFram].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	sprites_[SpriteNames::kStarFram]->SetSize({ sprites_[SpriteNames::kStarFram]->GetSize().x * 0.8f, sprites_[SpriteNames::kStarFram]->GetSize().y });

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = {};
		}
	}

	for (size_t i = 0; i < MapChip::Stage::kCount; i++) {
		std::array<bool, StageData::kMaxCondition> date{ false,false,false };
		condition_.emplace_back(date);
	}

	stageTeces_[0] = TextureManager::Load("Resources/Textures/stage1.png");
	stageTeces_[1] = TextureManager::Load("Resources/Textures/stage2.png");
	stageTeces_[2] = TextureManager::Load("Resources/Textures/stage3.png");
	stageTeces_[3] = TextureManager::Load("Resources/Textures/stage4.png");
	stageTeces_[4] = TextureManager::Load("Resources/Textures/stage5.png");
	stageTeces_[5] = TextureManager::Load("Resources/Textures/stage6.png");
	stageTeces_[6] = TextureManager::Load("Resources/Textures/stage7.png");
	stageTeces_[7] = TextureManager::Load("Resources/Textures/stage8.png");
	stageTeces_[8] = TextureManager::Load("Resources/Textures/stage9.png");
	stageTeces_[9] = TextureManager::Load("Resources/Textures/stage10.png");
	stageTeces_[10] = TextureManager::Load("Resources/Textures/stage11.png");
	stageTeces_[11] = TextureManager::Load("Resources/Textures/stage12.png");
	stageTeces_[12] = TextureManager::Load("Resources/Textures/stage13.png");
	stageTeces_[13] = TextureManager::Load("Resources/Textures/stage14.png");
	stageTeces_[14] = TextureManager::Load("Resources/Textures/stage15.png");
	stageTeces_[15] = TextureManager::Load("Resources/Textures/stageEX.png");

	starTeces_[StarState::kNotGet] = TextureManager::Load("Resources/Textures/starNotGet.png");
	starTeces_[StarState::kGet] = TextureManager::Load("Resources/Textures/starGet.png");

	for (int i = 0; i < 5; i++) {
		stages_[i].reset(Sprite::Create(stageTeces_[0], Vector2{ 640.0f,360.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	}
	for (int i = 0; i < 3; i++) {
		stars_[i].reset(Sprite::Create(starTeces_[StarState::kNotGet], Vector2{ 640.0f,360.0f }, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));
	}

	kStageTexSize_ = stages_[0]->GetSize().x;

	nowStage_ = 0;

	choiceSoundHandle_ = Audio::GetInstance()->SoundLoadWave("SE/choice.wav");
	selectSoundHandle_ = Audio::GetInstance()->SoundLoadWave("SE/select.wav");

	timer_ = std::make_unique<Timer>();
	timer_->SetStageNo(&nowStage_);
	timer_->Init();
	timer_->Update();
	drawMaxStage_ = 14;
}

void SelectSprites::Init() {
	SetGlobalVariable();

	nowStage_ = IScene::stageNo_;

	bool isClearAll = true;

	for (int stage = 0; stage < MapChip::Stage::kCount; stage++) {
		if (StageData::GetBestClearFlag(stage)) {
			// クリアしているか
			condition_.at(stage).at(0) = StageData::GetClearFlag(stage);

			// 時間
			if (StageData::GetBestClearTime(stage) <= StageData::GetConditionTime(stage)) {
				condition_.at(stage).at(1) = true;
			}
			else {
				condition_.at(stage).at(1) = false;
			}
			// アイテム数
			if (StageData::GetBestClearItemCount(stage) >= StageData::GetBestConditionItemCount(stage)) {
				condition_.at(stage).at(2) = true;
			}
			else {
				condition_.at(stage).at(2) = false;
			}
		}
		else {
			condition_.at(stage).at(0) = false;
			condition_.at(stage).at(1) = false;
			condition_.at(stage).at(2) = false;
		}

		if (stage <= drawMaxStage_) {
			if (!StageData::GetBestClearFlag(stage)) {
				isClearAll = false;
			}
		}
	}

	if (isClearAll) {
		if (drawMaxStage_ != kMaxStage_) {
			drawMaxStage_++;
		}
	}

	state_ = State::kSelect;
	stateRequest_ = State::kSelect;

	timer_->Init();

	SetStageNo();
}

void (SelectSprites::* SelectSprites::spStateInitFuncTable[])() {
	&SelectSprites::SelectInitialize,
		& SelectSprites::MoveInitialize,
};

void (SelectSprites::* SelectSprites::spStateUpdateFuncTable[])() {
	&SelectSprites::SelectUpdate,
		& SelectSprites::MoveUpdate,
};

void SelectSprites::Update() {

	ApplyGlobalVariable();


	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		stateRequest_ = std::nullopt;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();

	timer_->Update();
}

void SelectSprites::FarDraw() {
	blackBackground_->Draw();
}

void SelectSprites::NearDraw() {
	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		if (i != SpriteNames::kStarFram) {
			sprites_[i]->Draw();
		}
	}

	for (int i = 0; i < 5; i++) {
		stages_[i]->Draw();
	}

	for (int i = 0; i < 3; i++) {
		if (condition_.at(nowStage_).at(i)) {
			stars_[i]->SetTextureHandle(starTeces_[StarState::kGet]);
		}
		else {
			stars_[i]->SetTextureHandle(starTeces_[StarState::kNotGet]);
		}
		stars_[i]->Draw();
	}

	timer_->DrawUI();
}

void SelectSprites::StateRequest(State state) {
	if (state_ != state) {
		stateRequest_ = state;
	}
}

void SelectSprites::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	for (int i = 0; i < FItemNames::kFItemCount; i++) {
		globalVariables->AddItem(groupName_, fItemNames_[i], fInfo_[i]);
	}

	for (int i = 0; i < IItemNames::kIItemCount; i++) {
		globalVariables->AddItem(groupName_, iItemNames_[i], iInfo_[i]);
	}

	ApplyGlobalVariable();
}

void SelectSprites::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
	}

	for (int i = 0; i < FItemNames::kFItemCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fItemNames_[i]);
	}

	for (int i = 0; i < IItemNames::kIItemCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iItemNames_[i]);
	}

	stageBasePos_ = { 640.0f, fInfo_[FItemNames::kStagePosY] };
	starBasePos_ = { 640.0f,fInfo_[FItemNames::kStarPosY] };

	for (int i = 0; i < 3; i++) {
		stars_[i]->SetPosition({ starBasePos_.x - fInfo_[FItemNames::kStarWidthInterval] * (1 - i), starBasePos_.y });
	}
	for (int i = 0; i < 5; i++) {
		stages_[i]->SetPosition({ stageBasePos_.x - fInfo_[FItemNames::kStageWidthInterval] * (2 - i), stageBasePos_.y });
	}
}

void SelectSprites::SetStageNo() {
	for (int i = 0; i < 5; i++) {
		if (nowStage_ - 2 + i < 0) {
			stages_[i]->SetTextureHandle(stageTeces_[drawMaxStage_ + nowStage_ - 1 + i]);
		}
		else if (nowStage_ - 2 + i > drawMaxStage_) {
			if (nowStage_ == drawMaxStage_ - 1) {
				stages_[i]->SetTextureHandle(stageTeces_[i - 4]);
			}
			else {
				stages_[i]->SetTextureHandle(stageTeces_[i - 3]);
			}
		}
		else {
			stages_[i]->SetTextureHandle(stageTeces_[nowStage_ - 2 + i]);
		}

		if (i == 2) {
			float scale = kStageTexSize_;
			stages_[i]->SetSize({ scale,scale });
		}
		else {
			float scale = kStageTexSize_ * fInfo_[FItemNames::kStageSmallScale];
			stages_[i]->SetSize({ scale,scale });
		}
	}
}

void SelectSprites::TransformationInit() {
	countFrame_ = 0;
	scaleTheta_ = 0.0f;
}

void SelectSprites::TransformationUpdate() {
	const float pi = std::numbers::pi_v<float>;

	const float step = 2.0f * pi / iInfo_[IItemNames::kTransformationFrame];

	scaleTheta_ += step;

	scaleTheta_ = std::fmod(scaleTheta_, 2.0f * pi);

	const float amplitude = 15.0f;

	float scale = std::sinf(scaleTheta_) * amplitude;

	//stages_[2]->SetPosition({ stageBasePos_.x,stageBasePos_.y + scale });
	stages_[2]->SetSize({ kStageTexSize_ + scale,kStageTexSize_ + scale });
}

void SelectSprites::SelectInitialize() {
	SetStageNo();
	TransformationInit();
}

void SelectSprites::SelectUpdate() {
	Input* input = Input::GetInstance();

	TransformationUpdate();

	if (input->PressedGamePadButton(Input::GamePadButton::A)) {
		auto playHandle = Audio::GetInstance()->SoundPlayWave(choiceSoundHandle_);
		Audio::GetInstance()->SetValume(playHandle, 1.5f);
		// シーン切り替え
		IScene::stageNo_ = nowStage_;
		IScene::sceneNo_ = STAGE;
	}
	else if (input->GetGamePadLStick().x >= 0.3) {
		auto playHandle = Audio::GetInstance()->SoundPlayWave(selectSoundHandle_);
		Audio::GetInstance()->SetValume(playHandle, 0.3f);

		StateRequest(State::kMove);
		isRight_ = true;
	}
	else if (input->GetGamePadLStick().x <= -0.3) {
		auto playHandle = Audio::GetInstance()->SoundPlayWave(selectSoundHandle_);
		Audio::GetInstance()->SetValume(playHandle, 0.3f);

		StateRequest(State::kMove);
		isRight_ = false;
	}
}

void SelectSprites::MoveInitialize() {
	countFrame_ = 0;
}

void SelectSprites::MoveUpdate() {

	if (isRight_) {
		for (int i = 0; i < 5; i++) {
			stages_[i]->SetPosition({ Ease::UseEase(stageBasePos_.x - fInfo_[FItemNames::kStageWidthInterval] * (2 - i),
				stageBasePos_.x - fInfo_[FItemNames::kStageWidthInterval] * (3 - i),
				countFrame_,iInfo_[IItemNames::kMaxMoveFrame],Ease::EaseType::Constant), stageBasePos_.y });

			if (i == 2) {
				float scale = Ease::UseEase(kStageTexSize_, kStageTexSize_ * fInfo_[FItemNames::kStageSmallScale], countFrame_, iInfo_[IItemNames::kMaxMoveFrame], Ease::EaseType::Constant);
				stages_[i]->SetSize({ scale,scale });
			}
			else if (i == 3) {
				float scale = Ease::UseEase(kStageTexSize_ * fInfo_[FItemNames::kStageSmallScale], kStageTexSize_, countFrame_, iInfo_[IItemNames::kMaxMoveFrame], Ease::EaseType::Constant);
				stages_[i]->SetSize({ scale,scale });
			}
		}
	}
	else {
		for (int i = 0; i < 5; i++) {
			stages_[i]->SetPosition({ Ease::UseEase(stageBasePos_.x - fInfo_[FItemNames::kStageWidthInterval] * (2 - i),
				stageBasePos_.x - fInfo_[FItemNames::kStageWidthInterval] * (1 - i),
				countFrame_,iInfo_[IItemNames::kMaxMoveFrame],Ease::EaseType::Constant), stageBasePos_.y });
			if (i == 2) {
				float scale = Ease::UseEase(kStageTexSize_, kStageTexSize_ * fInfo_[FItemNames::kStageSmallScale], countFrame_, iInfo_[IItemNames::kMaxMoveFrame], Ease::EaseType::Constant);
				stages_[i]->SetSize({ scale,scale });
			}
			else if (i == 1) {
				float scale = Ease::UseEase(kStageTexSize_ * fInfo_[FItemNames::kStageSmallScale], kStageTexSize_, countFrame_, iInfo_[IItemNames::kMaxMoveFrame], Ease::EaseType::Constant);
				stages_[i]->SetSize({ scale,scale });
			}
		}
	}

	countFrame_++;

	if (countFrame_ >= iInfo_[IItemNames::kMaxMoveFrame]) {
		if (isRight_) {
			nowStage_++;
			if (nowStage_ > drawMaxStage_) {
				nowStage_ = 0;
			}
		}
		else {
			nowStage_--;
			if (nowStage_ < 0) {
				nowStage_ = drawMaxStage_;
			}
		}

		StateRequest(State::kSelect);
	}
}
