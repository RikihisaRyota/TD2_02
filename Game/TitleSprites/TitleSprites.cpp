#include "TitleSprites.h"

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
}

void TitleSprites::Update() {
	flashingCount_--;
	if (flashingCount_ <= 0) {
		if (on_) {
			sprites_[SpriteNames::kTitle]->SetTextureHandle(testTextureHandle_[kOn]);
			flashingCount_ = rnd_.NextIntRange(onMin_, onMax_);
			//CreateParticle();
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
		ImGui::DragFloat3("scale", &worldTransform_.scale_.x, 0.1f);
		ImGui::DragFloat3("rotate", &worldTransform_.rotation_.x, 0.01f);
		ImGui::DragFloat3("pos", &worldTransform_.translate_.x, 1.0f);
		worldTransform_.UpdateMatrix();
		for (auto& pos : worldTransforms_) {
			pos.UpdateMatrix();
		}
	}
	ImGui::End();
	ApplyGlobalVariable();
#endif // _DEBUG
}

void TitleSprites::FarDraw() {
	sprites_[SpriteNames::kBackground]->Draw();
}

void TitleSprites::Draw(const ViewProjection& viewProjection) {
	/*for (int i = 0; auto & model:models_) {
		model->Draw(worldTransforms_.at(i++), viewProjection);
	}*/
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
		emitter->spawn.rangeX = 0.0f;
		emitter->spawn.rangeY = 0.0f;
		emitter->scale.startScale = { 0.0f,0.0f,0.0f };
		emitter->scale.interimScale = { 5.0f,5.0f,5.0f };
		emitter->scale.endScale = { 0.0f,0.0f,0.0f };

		emitter->color.startColor_ = { 0.6f,0.6f,0.1f,1.0f };
		emitter->color.endColor_ = { 0.6f, 0.6f, 0.1f, 1.0f };

		emitter->inOnce = 50;
		emitter->angle.start = DegToRad(0.0f);
		emitter->angle.end = DegToRad(360.0f);
		emitter->isAlive = true;

		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 2.0f;
		particleMotion->velocity.randomRange = 0.0f;

		particleMotion->aliveTime.time = 60;
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
