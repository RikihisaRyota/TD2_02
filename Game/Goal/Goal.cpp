#include "Goal.h"

#include "GlobalVariables/GlobalVariables.h"
#include "SceneSystem/IScene/IScene.h"
#include "TextureManager.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"
#include "ParticleManager.h"
#include "MyMath.h"
#include "ImGuiManager.h"

Goal::Goal() {
	model_ = std::make_unique<PlaneRenderer>();
	model_.reset(PlaneRenderer::Create());
	auto material = model_->GetMaterial();
	material->color_ = { 0.2f,0.2f ,0.2f ,0.2f };
	model_->SetMaterial(*material);
	goalTexture_ = TextureManager::Load("Resources/Textures/goal.png");
	particleTexture_ = TextureManager::Load("Resources/Textures/circle.png");
	worldTransform_.Initialize();

	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	SetCollisionAttribute(kCollisionAttributeGoal);
	SetCollisionMask(kCollisionAttributePlayer);
	aliveTime_ = 10.0f;
	angle_ = 0.0f;
	addAngle_ = 5.0f;
	speed_ = 0.1f;
	SetGlobalVariable();
	worldTransform_.UpdateMatrix();
	ParticleInitialize();
}

void Goal::Initialize() {
	aliveTime_ = 10.0f;
	angle_ = 0.0f;
	addAngle_ = 5.0f;
	speed_ = 0.1f;
	SetGlobalVariable();
	worldTransform_.UpdateMatrix();
	ParticleInitialize();
	
}

void Goal::Update() {
	static const float kColorMax = 120.0f;
	colorTime_ += 1.0f;
	if (colorTime_ >= kColorMax) {
		colorTime_  = 0.0f;
	}
	float t = colorTime_ / kColorMax;
	auto material = model_->GetMaterial();
	if (colorTime_ <= kColorMax / 2.0f) {
		material->color_ = Lerp(Vector4(0.5f, 0.5f, 0.5f, 0.5f), Vector4(0.7f, 0.7f, 0.7f, 0.7f), std::clamp(t, 0.0f,1.0f));
	}
	else {
		material->color_ = Lerp(Vector4(0.7f, 0.7f, 0.7f, 0.7f), Vector4(0.5f, 0.5f, 0.5f, 0.5f), std::clamp(t, 0.0f, 1.0f));
	}
	model_->SetMaterial(*material);

	angle_ -= addAngle_;
	if (angle_ <= -360.0f) {
		angle_ = 0.0f;
	}
	CreateParticle();
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG

	SetCollider();
}

void Goal::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, goalTexture_);
}

void Goal::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < MapChip::Stage::kCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void Goal::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < MapChip::Stage::kCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector3Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}
	}
	worldTransform_.translate_ = v2Info_[IScene::stageNo_][V2ItemNames::kPos];
	worldTransform_.scale_ = v2Info_[IScene::stageNo_][V2ItemNames::kScale];
	worldTransform_.UpdateMatrix();
}

void Goal::OnCollision() {

}

void Goal::SetCollider() {
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ worldTransform_.scale_.x,worldTransform_.scale_.y }, Vector2{});

	CollisionManager::GetInstance()->SetCollider(this);
}

void Goal::ParticleInitialize() {
	for (int i = 0; i < kNumParticle; i++) {
		emitter_[i] = new Emitter();
		emitter_[i]->aliveTime = 600;
		emitter_[i]->spawn.position = worldTransform_.worldPos_;
		emitter_[i]->spawn.rangeX = 0.0f;
		emitter_[i]->spawn.rangeY = 0.0f;
		emitter_[i]->scale.startScale = { 1.0f,1.0f,1.0f };
		emitter_[i]->scale.endScale = { 0.01f,0.01f,0.01f };
		emitter_[i]->inOnce = 1;
		emitter_[i]->angle.start = 0.0f;
		emitter_[i]->angle.end = 0.0f;
		emitter_[i]->isAlive = true;
		particleMotion_[i] = new ParticleMotion();
		particleMotion_[i]->color.startColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.75f,0.8f),rnd_.NextFloatRange(0.8f,1.0f) };
		particleMotion_[i]->color.endColor = { rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.0f) };
		particleMotion_[i]->color.currentColor = particleMotion_[i]->color.startColor;
		particleMotion_[i]->rotate.addRotate = { 0.0f,0.0f,0.0f };
		particleMotion_[i]->rotate.currentRotate = { 0.0f,0.0f,0.0f };
		particleMotion_[i]->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion_[i]->velocity.speed = speed_;
		particleMotion_[i]->velocity.randomRange = 0.0f;
		particleMotion_[i]->aliveTime.time = int32_t(worldTransform_.scale_.x * aliveTime_);
		particleMotion_[i]->aliveTime.randomRange = 0;
		particleMotion_[i]->isAlive = true;
		ParticleManager::GetInstance()->AddParticle(emitter_[i], particleMotion_[i], particleTexture_);
	}
}

void Goal::CreateParticle() {
	for (int i = 0; i < kNumParticle; i++) {
		emitter_[i]->aliveTime = 1;
		float angle = (float(i) / float(kNumParticle) * 360.0f);
		emitter_[i]->spawn.position.x = worldTransform_.translate_.x + std::cos(DegToRad(angle_ + angle)) * worldTransform_.scale_.x;
		emitter_[i]->spawn.position.y = worldTransform_.translate_.y + std::sin(DegToRad(angle_ + angle)) * worldTransform_.scale_.y;
		emitter_[i]->angle.current = DegToRad(angle_ + angle);
		particleMotion_[i]->velocity.speed = -speed_;
		particleMotion_[i]->color.startColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.75f,0.8f),rnd_.NextFloatRange(0.2f,0.5f) };
		particleMotion_[i]->color.endColor = { rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.0f) };
		particleMotion_[i]->aliveTime.time = int32_t(worldTransform_.scale_.x * aliveTime_);
	}
}
