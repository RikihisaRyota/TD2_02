#include "TitleSprites.h"

#include <numbers>

#include "Audio.h"
#include "TextureManager.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Input.h"
#include "ImGuiManager.h"
#include "ParticleShaderStruct.h"
#include "ParticleUIManager.h"
#include "MyMath.h"
#include "ModelManager.h"


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

	models_.emplace_back((ModelManager::GetInstance()->GetModel("player")));
	models_.emplace_back((ModelManager::GetInstance()->GetModel("playerTail")));
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 2.5f,2.5f,2.5f };
	worldTransform_.rotation_ = { 0.0f,2.5f,0.0f };
	worldTransform_.translate_ = { 55.0f,10.0f,0.0f };
	worldTransform_.UpdateMatrix();

	for (auto model : models_) {
		auto material = model->GetMaterial()->GetMaterial();
		material->color_ = { 1.0f,1.0f,1.0f,1.0f };
		model->GetMaterial()->SetMaterial(*material);
	}

	worldTransforms_.emplace_back(WorldTransform());
	worldTransforms_.back().Initialize();
	worldTransforms_.back().parent_ = &worldTransform_;
	worldTransforms_.back().UpdateMatrix();

	worldTransforms_.emplace_back(WorldTransform());
	worldTransforms_.back().Initialize();
	worldTransforms_.back().parent_ = &worldTransform_;
	worldTransforms_.back().translate_ = { -0.9f, -0.1f, 0.0f };
	worldTransforms_.back().rotation_= { 0.0f, 0.5f, 0.0f };
	worldTransforms_.back().UpdateMatrix();

	flashingCount_ = rnd_.NextIntRange(onMin_, onMax_);
	on_ = true;
	onMin_ = 20;
	onMax_ = 300;
	offMin_ = 5;
	offMax_ = 15;
	time_ = 0.0f;
}

void TitleSprites::Init() {
	SetGlobalVariable();
	flashingCount_ = rnd_.NextIntRange(onMin_, onMax_);
	on_ = true;
	for (auto model : models_) {
		auto material = model->GetMaterial()->GetMaterial();
		material->color_ = { 1.0f,1.0f,1.0f,1.0f };
		model->GetMaterial()->SetMaterial(*material);
	}
	time_ = 0.0f;
	kMaxTime_ = 60.0f;
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

	const uint16_t cycle = 60;

	const float pi = std::numbers::pi_v<float>;

	const float step = 2.0f * pi / cycle;

	theta_ += step;

	theta_ = std::fmod(theta_, 2.0f * pi);
	
	const float amplitude = 0.2f;

	float rotate = std::sinf(theta_) * amplitude;

	worldTransform_.rotation_.y = 2.5f + rotate;
	worldTransform_.UpdateMatrix();
	for (auto& pos : worldTransforms_) {
		pos.UpdateMatrix();
	}

	time_ += 1.0f;
	if (time_ >= kMaxTime_) {
		CreateParticle();
		time_ = 0.0f;
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
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("pos", &worldTransform_.translate_.x, 1.0f);
		
	}
	ImGui::End();
	ApplyGlobalVariable();
#endif // _DEBUG
}

void TitleSprites::FarDraw() {
	sprites_[SpriteNames::kBackground]->Draw();
}

void TitleSprites::Draw(const ViewProjection& viewProjection) {
	for (int i = 0; auto & model:models_) {
		model->Draw(worldTransforms_.at(i++), viewProjection);
	}
}

void TitleSprites::NearDraw() {
	sprites_[SpriteNames::kTitle]->Draw();
	sprites_[SpriteNames::kDecisionA]->Draw();
}

void TitleSprites::CreateParticle() {
	{
		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();

		emitter->aliveTime = 1;
		emitter->flameInterval = 0;
		emitter->spawn.position = Vector3(rnd_.NextFloatRange(0.0f, 1280.0f), rnd_.NextFloatRange(0.0f, 720.0f), 0.0f);
		emitter->spawn.rangeX = 300.0f;
		emitter->spawn.rangeY = 300.0f;
		emitter->scale.startScale = { 0.0f,0.0f,0.0f };
		float scale = rnd_.NextFloatRange(150.0f,400.0f);
		emitter->scale.interimScale = { scale ,scale ,scale };
		emitter->scale.endScale = { 0.0f,0.0f,0.0f };

		//emitter->color.startColor_ = { 0.6f,0.6f,0.1f,1.0f };
		emitter->color.startBeginMinRandomColor_ = { 0.1f,0.1f ,0.1f ,0.2f };
		emitter->color.startBeginMaxRandomColor_ = { 0.2f,0.2f,0.2f,0.2f };
		emitter->color.endColor_ = { 0.0f, 0.0f, 0.0f, 1.0f };

		emitter->inOnce = 4;
		emitter->angle.start = DegToRad(0.0f);
		emitter->angle.end = DegToRad(360.0f);
		emitter->isAlive = true;

		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.02f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 0.0f;
		particleMotion->velocity.randomRange = 0.0f;

		particleMotion->aliveTime.time = 120;
		particleMotion->aliveTime.randomRange = 0;
		particleMotion->isAlive = true;
		ParticleUIManager::GetInstance()->AddParticle(emitter, particleMotion, 0);
	}
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
