#include "ClearSprites.h"

#include "Audio.h"
#include "Ease/Ease.h"
#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Game/StageData/StageData.h"
#include "MapChip.h"
#include "SceneSystem/IScene/IScene.h"
#include "ParticleUIManager.h"
#include "MyMath.h"
#include "ImGuiManager.h"


ClearSprites::ClearSprites() {
	input_ = Input::GetInstance();

	uint32_t tex = TextureManager::Load("Resources/Textures/white1x1.png");
	sprites_[SpriteNames::kBackground].reset(Sprite::Create(tex, Vector2{}));
	sprites_[SpriteNames::kBackground]->SetColor({ 0.0f,0.0f,0.0f,1.0f });
	sprites_[SpriteNames::kBackground]->SetSize({ 1280.0f,720.0f });

	tex = TextureManager::Load("Resources/Textures/result.png");
	sprites_[SpriteNames::kResult].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/goal.png");
	sprites_[SpriteNames::kGoal].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/clear.png");
	sprites_[SpriteNames::kClear].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/item.png");
	sprites_[SpriteNames::kItem].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/time.png");
	sprites_[SpriteNames::kTime].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/time0.png");
	sprites_[SpriteNames::kTimeOnesPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeTensPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeHundredsPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/resultTime0.png");
	sprites_[SpriteNames::kTimeConditionOnesPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeConditionTensPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kTimeConditionHundredsPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/time0.png");
	sprites_[SpriteNames::kItemOnesPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kItemTensPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/resultTime0.png");
	sprites_[SpriteNames::kItemConditionOnesPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kItemConditionTensPlace].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/slash.png");
	sprites_[SpriteNames::kSlash0].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kSlash1].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	tex = TextureManager::Load("Resources/Textures/starGet.png");
	sprites_[SpriteNames::kStarFirst].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kStarSecond].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));
	sprites_[SpriteNames::kStarThird].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));

	endStarSize_ = sprites_[SpriteNames::kStarFirst]->GetSize();
	startStarSize_ = endStarSize_ * 3.0f;

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

	tex = TextureManager::Load("Resources/Textures/retry.png");
	sprites_[SpriteNames::kRetry].reset(Sprite::Create(tex, Vector2{}, Vector4{ 1.0f,1.0f,1.0f,1.0 }, Vector2{ 0.5f,0.5f }));


	for (uint32_t i = 0; i < kNumberCount; i++) {
		number_[i] = TextureManager::Load("Resources/Textures/time" + std::to_string(i) + ".png");
		conditionNumber_[i] = TextureManager::Load("Resources/Textures/resultTime" + std::to_string(i) + ".png");
	}

	star_[kTrue] = TextureManager::Load("Resources/Textures/starGet.png");
	star_[kFalse] = TextureManager::Load("Resources/Textures/starNotGet.png");

	nextStage_[kTrue] = TextureManager::Load("Resources/Textures/goNextStageChoice.png");
	nextStage_[kFalse] = TextureManager::Load("Resources/Textures/goNextStage.png");

	selectStage_[kTrue] = TextureManager::Load("Resources/Textures/goStageSelectChoice.png");
	selectStage_[kFalse] = TextureManager::Load("Resources/Textures/goStageSelect.png");

	retry_[kTrue] = TextureManager::Load("Resources/Textures/retrySelect.png");
	retry_[kFalse] = TextureManager::Load("Resources/Textures/retry.png");

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

	currentStageNo_ = 0;

	speed_ = 15.0f;
	acceleration_ = 0.5f;
	position_ = { 640.0f,-10.0f };
}

void ClearSprites::Init() {
	isAnimation_ = true;
	animationCount_ = 0.0f;
	kMaxAnimationCount_ = 60.0f;
	firstStarCount_ = 0.0f;
	secondStarCount_ = 0.0f;
	thirdStarCount_ = 0.0f;
	kMaxStarCount_ = 15.0f;
	for (auto& flag : createFlag_) {
		flag = false;
	}
	sprites_[SpriteNames::kStarFirst]->SetTextureHandle(star_[kFalse]);
	sprites_[SpriteNames::kStarSecond]->SetTextureHandle(star_[kFalse]);
	sprites_[SpriteNames::kStarThird]->SetTextureHandle(star_[kFalse]);

	currentStageNo_ = IScene::stageNo_;
	SetGlobalVariable();
	if (IScene::stageNo_ != MapChip::kCount - 1) {
		state_ = State::kNextStageState;
	}
	else {
		state_ = State::kSelectStageState;
	}
	// 秒に直す
	int clearTime = StageData::GetClearTime(currentStageNo_) / 60;
	int conditionTime = StageData::GetConditionTime(currentStageNo_) / 60;
	int place = 100;
	if (clearTime >= 999) {
		for (int i = 0; i < 3; i++) {
			timePlace_[i] = 9;
			conditionTimePlace_[2 - i] = conditionTime / place;
			clearTime %= place;
			conditionTime %= place;
			place /= 10;
		}
	}
	else {
		for (int i = 0; i < 3; i++) {
			timePlace_[2 - i] = clearTime / place;
			conditionTimePlace_[2 - i] = conditionTime / place;
			clearTime %= place;
			conditionTime %= place;
			place /= 10;
		}
	}
	// アイテム数
	int clearItem = StageData::GetClearItemCount(currentStageNo_);
	int conditionItem = StageData::GetConditionItemCount(currentStageNo_);
	place = 10;
	for (int i = 0; i < 2; i++) {
		itemPlace_[1 - i] = clearItem / place;
		conditionItemPlace_[1 - i] = conditionItem / place;
		clearItem %= place;
		conditionItem %= place;
		place /= 10;
	}
	// 条件1
	if (StageData::GetClearFlag(currentStageNo_)) {
		starFlag_[0] = true;
	}
	else {
		starFlag_[0] = false;
	}
	// 条件2
	if (StageData::GetClearTime(currentStageNo_) <= StageData::GetConditionTime(currentStageNo_)) {
		starFlag_[1] = true;
	}
	else {
		starFlag_[1] = false;
	}
	// 条件3
	if (StageData::GetClearItemCount(currentStageNo_) >= StageData::GetConditionItemCount(currentStageNo_)) {
		starFlag_[2] = true;
	}
	else {
		starFlag_[2] = false;
	}
}

void ClearSprites::Update() {
	if (!isAnimation_) {
		if (currentStageNo_ != MapChip::kCount - 1 &&
			input_->GetGamePadLStick().x < 0.0f &&
			input_->GetPreGamePadLStick().x == 0.0f) {
			switch (state_) {
			case ClearSprites::State::kSelectStageState:
				state_ = State::kNextStageState;
				break;
			case ClearSprites::State::kRetryState:
				state_ = State::kSelectStageState;
				break;
			case ClearSprites::State::kNextStageState:
				state_ = State::kRetryState;
				break;
			}
		}
		if (currentStageNo_ != MapChip::kCount - 1 &&
			input_->GetGamePadLStick().x > 0.0f &&
			input_->GetPreGamePadLStick().x == 0.0f) {
			switch (state_) {
			case ClearSprites::State::kSelectStageState:
				state_ = State::kRetryState;
				break;
			case ClearSprites::State::kRetryState:
				state_ = State::kNextStageState;
				break;
			case ClearSprites::State::kNextStageState:
				state_ = State::kSelectStageState;
				break;
			}
		}
	}
	if (input_->PressedGamePadButton(Input::GamePadButton::A)) {
		animationCount_ = kMaxAnimationCount_;
		firstStarCount_ = kMaxStarCount_;
		secondStarCount_ = kMaxStarCount_;
		thirdStarCount_ = kMaxStarCount_;
	}
	animationCount_ += 1.0f;
	if (animationCount_ >= kMaxAnimationCount_) {
		if (starFlag_[2]) {
			sprites_[SpriteNames::kStarThird]->SetTextureHandle(star_[kTrue]);
			if (thirdStarCount_ <= kMaxStarCount_) {
				thirdStarCount_ += 1.0f;
				float t = thirdStarCount_ / kMaxStarCount_;
				sprites_[SpriteNames::kStarThird]->SetSize(Ease::UseEase(startStarSize_, endStarSize_, thirdStarCount_, kMaxStarCount_, Ease::EaseType::EaseOutBounce));
				//sprites_[SpriteNames::kStarThird]->SetRotation(Lerp(1080.0f, 0.0f, std::clamp(t, 0.0f, 1.0f)));
			}
			else if (!createFlag_[2]) {
				CreateCompleteParticle();
				CreateParticle(sprites_[SpriteNames::kStarThird]->GetPosition());
				createFlag_[2] = true;
			}
		}
		else {
			sprites_[SpriteNames::kStarThird]->SetTextureHandle(star_[kFalse]);
		}
		isAnimation_ = false;
	}
	else {
		sprites_[SpriteNames::kStarThird]->SetTextureHandle(star_[kFalse]);
	}
	if (animationCount_ >= kMaxAnimationCount_ * 2.0f / 3.0f) {
		if (starFlag_[1]) {
			if (secondStarCount_ <= kMaxStarCount_) {
				sprites_[SpriteNames::kStarSecond]->SetTextureHandle(star_[kTrue]);
				secondStarCount_ += 1.0f;
				float t = secondStarCount_ / kMaxStarCount_;
				sprites_[SpriteNames::kStarSecond]->SetSize(Ease::UseEase(startStarSize_, endStarSize_, secondStarCount_, kMaxStarCount_, Ease::EaseType::EaseOutBounce));
				//sprites_[SpriteNames::kStarSecond]->SetRotation(Lerp(1080.0f, 0.0f, std::clamp(t, 0.0f, 1.0f)));
			}
			else if (!createFlag_[1]) {
				CreateParticle(sprites_[SpriteNames::kStarSecond]->GetPosition());
				createFlag_[1] = true;
			}
		}
		else {
			sprites_[SpriteNames::kStarSecond]->SetTextureHandle(star_[kFalse]);
		}
	}
	else {
		sprites_[SpriteNames::kStarSecond]->SetTextureHandle(star_[kFalse]);
	}
	if (animationCount_ >= kMaxAnimationCount_ * 1.0f / 3.0f) {
		if (starFlag_[0]) {
			sprites_[SpriteNames::kStarFirst]->SetTextureHandle(star_[kTrue]);
			if (firstStarCount_ <= kMaxStarCount_) {
				firstStarCount_ += 1.0f;
				float t = firstStarCount_ / kMaxStarCount_;
				sprites_[SpriteNames::kStarFirst]->SetSize(Ease::UseEase(startStarSize_, endStarSize_, firstStarCount_, kMaxStarCount_, Ease::EaseType::EaseOutBounce));
				//sprites_[SpriteNames::kStarFirst]->SetRotation(Lerp(1080.0f, 0.0f, std::clamp(t, 0.0f, 1.0f)));
			}
			else if (!createFlag_[0]) {
				CreateParticle(sprites_[SpriteNames::kStarFirst]->GetPosition());
				createFlag_[0] = true;
			}
		}
		else {
			sprites_[SpriteNames::kStarFirst]->SetTextureHandle(star_[kFalse]);
		}
	}
	else {
		sprites_[SpriteNames::kStarFirst]->SetTextureHandle(star_[kFalse]);
	}
	if(input_->TriggerKey(DIK_SPACE)){
		CreateCompleteParticle();
	}
	if(ImGui::TreeNode("Clear")){
		ImGui::DragFloat("speed", &speed_, 0.1f);
		ImGui::DragFloat("acceleration", &acceleration_, 0.1f);
		ImGui::DragFloat2("position", &position_.x, 1.0f);
		ImGui::TreePop();

	}
	//#ifdef _DEBUG
	//	ApplyGlobalVariable();
	//#endif // _DEBUG
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
		case ClearSprites::kTimeConditionOnesPlace:
			sprite->SetTextureHandle(conditionNumber_[conditionTimePlace_[0]]);
			sprite->Draw();
			break;
		case ClearSprites::kTimeConditionTensPlace:
			sprite->SetTextureHandle(conditionNumber_[conditionTimePlace_[1]]);
			sprite->Draw();
			break;
		case ClearSprites::kTimeConditionHundredsPlace:
			sprite->SetTextureHandle(conditionNumber_[conditionTimePlace_[2]]);
			sprite->Draw();
			break;
		case ClearSprites::kItemOnesPlace:
			sprite->SetTextureHandle(number_[itemPlace_[0]]);
			sprite->Draw();
			break;
		case ClearSprites::kItemTensPlace:
			sprite->SetTextureHandle(number_[itemPlace_[1]]);
			sprite->Draw();
			break;
		case ClearSprites::kItemConditionOnesPlace:
			sprite->SetTextureHandle(conditionNumber_[conditionItemPlace_[0]]);
			sprite->Draw();
			break;
		case ClearSprites::kItemConditionTensPlace:
			sprite->SetTextureHandle(conditionNumber_[conditionItemPlace_[1]]);
			sprite->Draw();
			break;
		case ClearSprites::kStarFirst:
			sprite->Draw();
			break;
		case ClearSprites::kStarSecond:
			sprite->Draw();
			break;
		case ClearSprites::kStarThird:
			sprite->Draw();
			break;
		case ClearSprites::kSelectStage:
			if (currentStageNo_ == MapChip::Stage::kCount - 1) {
				sprite->SetTextureHandle(selectStage_[kTrue]);
				sprite->SetPosition(Vector2(640.0f, 600.0f));
			}
			else {
				if (state_ == State::kSelectStageState) {
					sprite->SetTextureHandle(selectStage_[kTrue]);
				}
				else {
					sprite->SetTextureHandle(selectStage_[kFalse]);
				}
				sprite->SetPosition(Vector2(248.0f, 600.0f));
			}
			sprite->Draw();
			break;
		case ClearSprites::kNextStage:
			if (currentStageNo_ != MapChip::kCount - 1) {
				if (state_ == State::kNextStageState) {
					sprite->SetTextureHandle(nextStage_[kTrue]);
				}
				else {
					sprite->SetTextureHandle(nextStage_[kFalse]);
				}
				sprite->Draw();

			}
			break;
		case ClearSprites::kRetry:
			if (currentStageNo_ != MapChip::kCount - 1) {
				if (state_ == State::kRetryState) {
					sprite->SetTextureHandle(retry_[kTrue]);
				}
				else {
					sprite->SetTextureHandle(retry_[kFalse]);
				}
				sprite->Draw();

			}
			break;
		case ClearSprites::kBackground:
		case ClearSprites::kResult:
		case ClearSprites::kGoal:
		case ClearSprites::kClear:
		case ClearSprites::kItem:
		case ClearSprites::kTime:
		case ClearSprites::kSlash0:
		case ClearSprites::kSlash1:
		case ClearSprites::kDecisionA:
		case ClearSprites::kSelectLS:
		case ClearSprites::kRank:
			sprite->Draw();
			break;
		}
		i++;
	}
}
void ClearSprites::ParticleInitialize() {}
void ClearSprites::ParticleUpdate() {}
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

void ClearSprites::CreateParticle(const Vector2& position) {
	{
		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();

		emitter->aliveTime = 1;
		emitter->flameInterval = 0;
		emitter->spawn.position = Vector3(position.x, position.y, 0.0f);
		emitter->spawn.rangeX = 0.0f;
		emitter->spawn.rangeY = 0.0f;
		emitter->scale.startScale = { 0.0f,0.0f,0.0f };
		emitter->scale.interimScale = { 10.0f,10.0f,10.0f };
		emitter->scale.endScale = { 0.0f,0.0f,10.0f };

		emitter->color.startColor_ = { 0.7f,0.7f,0.1f,1.0f };
		emitter->color.endColor_ = { 0.7f, 0.7f, 0.1f, 1.0f };

		emitter->inOnce = 50;
		emitter->angle.start = DegToRad(0.0f);
		emitter->angle.end = DegToRad(360.0f);
		emitter->isAlive = true;

		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 1.5f;
		particleMotion->velocity.randomRange = 0.0f;

		particleMotion->aliveTime.time = 60;
		particleMotion->aliveTime.randomRange = 0;
		particleMotion->isAlive = true;
		ParticleUIManager::GetInstance()->AddParticle(emitter, particleMotion, 0);

	}

	/*{
		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();

		emitter->aliveTime = 1;
		emitter->flameInterval = 0;
		emitter->spawn.position = Vector3(position.x, position.y, 0.0f);
		emitter->spawn.rangeX = 0.0f;
		emitter->spawn.rangeY = 0.0f;
		emitter->scale.startScale = { 5.0f,50.0f,5.0f };
		emitter->scale.interimScale = { 5.0f,50.0f,5.0f };
		emitter->scale.endScale = { 5.0f,50.0f,5.0f };

		emitter->inOnce = 50;
		emitter->angle.start = DegToRad(0.0f);
		emitter->angle.end = DegToRad(360.0f);
		emitter->isAlive = true;

		particleMotion->color.startColor = { 0.3f,0.3f,0.3f,1.0f };
		particleMotion->color.endColor = { 0.0f,0.0f,0.0f,1.0f };
		particleMotion->color.currentColor = particleMotion->color.startColor;

		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.1f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 2.0f;
		particleMotion->velocity.randomRange = 1.0f;

		particleMotion->aliveTime.time = 20;
		particleMotion->aliveTime.randomRange = 20;
		particleMotion->isAlive = true;
		ParticleUIManager::GetInstance()->AddParticle(emitter, particleMotion, 0);

	}*/
}

void ClearSprites::CreateCompleteParticle() {
	{
		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();

		emitter->aliveTime = 120;
		emitter->flameInterval = 3;
		emitter->spawn.position = Vector3(position_.x, position_.y, 0.0f);
		emitter->spawn.rangeX = 0.0f;
		emitter->spawn.rangeY = 0.0f;
		emitter->scale.startScale = { 20.0f,20.0f,20.0f };
		emitter->scale.endScale = { 20.0f,20.0f,20.0f };
		emitter->color.startColor_ = { 1.0f,1.0f,1.0f,1.0f };
		emitter->color.endColor_ = { 0.0f,0.0f,0.0f,1.0f };
		emitter->color.startBeginMinRandomColor_ = { 0.0f,0.0f,0.0f,1.0f };
		emitter->color.startBeginMaxRandomColor_ = { 1.0f,1.0f,1.0f,1.0f };
		
		emitter->inOnce = 5;
		emitter->angle.start = DegToRad(45.0f);
		emitter->angle.end = DegToRad(135.0f);
		emitter->isAlive = true;

		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.1f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->acceleration_ = { 0.0f,acceleration_,0.0f };
		particleMotion->velocity.speed = -speed_;
		particleMotion->velocity.randomRange = 1.0f;

		particleMotion->aliveTime.time = 600;
		particleMotion->aliveTime.randomRange = 0;
		particleMotion->isAlive = true;
		ParticleUIManager::GetInstance()->AddParticle(emitter, particleMotion, 0);

	}
}
