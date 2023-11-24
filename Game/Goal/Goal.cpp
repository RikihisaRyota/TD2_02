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
	goalTexture_ = TextureManager::Load("Resources/Textures/goal.png");
	particleTexture_ = TextureManager::Load("Resources/Textures/circle.png");

	worldTransform_.Initialize();

	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	SetCollisionAttribute(kCollisionAttributeGoal);
	SetCollisionMask(kCollisionAttributePlayer);
	aliveTime_ = 1.0f;
	angle_ = 0.0f;
	ParticleInitialize();
}

void Goal::Initialize() {
	aliveTime_ = 1.0f;
	angle_ = 0.0f;
	ParticleInitialize();
	SetGlobalVariable();
}

void Goal::Update() {
	angle_ += 1.0f;
	if (angle_ >= 360.0f) {
		angle_ = 0.0f;
	}
	CreateParticle();
	ImGui::Begin("Debug");
	if (ImGui::TreeNode("Goal")) {
		ImGui::DragFloat("aliveTime", &aliveTime_, 0.1f, 0.0f);
		ImGui::TreePop();
	}
	ImGui::End();
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

	for (int i = 0; i < GoalNames::kGoalCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void Goal::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < GoalNames::kGoalCount; i++) {
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
	emitter_ = new Emitter();
	emitter_->aliveTime = 2;
	emitter_->spawn.position = worldTransform_.worldPos_;
	emitter_->spawn.rangeX = 0.0f;
	emitter_->spawn.rangeY = 0.0f;
	emitter_->inOnce = 1;
	emitter_->angle.start = DegToRad(angle_);
	emitter_->angle.end = DegToRad(angle_);
	emitter_->isAlive = true;
	particleMotion_ = new ParticleMotion();
	particleMotion_->color.startColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.8f),rnd_.NextFloatRange(0.8f,1.0f) };
	particleMotion_->color.endColor = { rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.0f) };
	particleMotion_->color.currentColor = particleMotion_->color.startColor;
	particleMotion_->scale.startScale = { 1.0f,1.0f,1.0f };
	particleMotion_->scale.endScale = { 0.01f,0.01f,0.01f };
	particleMotion_->scale.currentScale = particleMotion_->scale.startScale;
	particleMotion_->rotate.addRotate = { 0.0f,0.0f,0.0f };
	particleMotion_->rotate.currentRotate = { 0.0f,0.0f,0.0f };
	particleMotion_->acceleration_ = { 0.0f,0.0f,0.0f };
	particleMotion_->velocity.speed = 0.1f;
	particleMotion_->velocity.randomRange = 0.0f;
	particleMotion_->aliveTime.time = int32_t(worldTransform_.scale_.x * aliveTime_);
	particleMotion_->aliveTime.randomRange = 0;
	particleMotion_->isAlive = true;
	ParticleManager::GetInstance()->AddParticle(emitter_, particleMotion_, particleTexture_);
}

void Goal::CreateParticle() {
	emitter_->aliveTime = 1;
	emitter_->spawn.position = worldTransform_.worldPos_;
	emitter_->angle.start = DegToRad(angle_);
	emitter_->angle.end = DegToRad(angle_);
	particleMotion_->color.startColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.8f),rnd_.NextFloatRange(0.0f,0.5f) };
	particleMotion_->color.endColor = { rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.0f) };
}
