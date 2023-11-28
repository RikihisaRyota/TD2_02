#include "Background.h"

#include "ImGuiManager.h"
#include "MyMath.h"
#include "ParticleManager.h"
#include "ParticleShaderStruct.h"
#include "Game/Player/Player.h"

Background::Background() {
	color_ = { 0.0f,0.0f,0.01f,1.0f };
	sprite_.reset(Sprite::Create(0, { 0.0f,0.0f }, color_, {0.5f,0.5f}));
	sprite_->SetSize({ 1280.0f*2.0f,720.0f*2.0f });
	worldTransform_.Initialize();
	// デバック用
	Initialize();
}

void Background::Initialize() {
	worldTransform_.translate_ = {98.0f,74.0f,160.0f};
	worldTransform_.UpdateMatrix();
	time_ = 0;
}

void Background::Update() {
	time_++;
	static const int kMaxTime = 60;
	if (time_>= kMaxTime) {
		Emitter* emitter = new Emitter();
		emitter->aliveTime = 1;
		emitter->spawn.position = MakeTranslate(player_->GetWorldTransform()->matWorld_);
		emitter->spawn.rangeX = 40.0f;
		emitter->spawn.rangeY = 40.0f;
		float scale = rnd_.NextFloatRange(10.0f, 20.0f);
		emitter->scale.startScale = { 0.01f,0.01f,0.01f };
		emitter->scale.interimScale = { scale,scale ,scale };
		emitter->scale.endScale = { 0.01f,0.01f,0.01f };
		emitter->inOnce = 4;
		emitter->angle.start = DegToRad(0.0f);
		emitter->angle.end = DegToRad(360.0f);
		emitter->isAlive = true;
		ParticleMotion* particleMotion = new ParticleMotion();
		particleMotion->color.startColor = { rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.8f,1.0f),rnd_.NextFloatRange(0.0f,0.2f) };
		particleMotion->color.endColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.4f),rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.0f,0.0f) };
		particleMotion->color.currentColor = particleMotion->color.startColor;
		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.02f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };
		particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
		particleMotion->velocity.speed = 0.2f;
		particleMotion->velocity.randomRange = 0.05f;
		particleMotion->aliveTime.time = 60;
		particleMotion->aliveTime.randomRange = 30;
		particleMotion->isAlive = true;
		particleMotion->isAlive = true;
		ParticleManager::GetInstance()->AddParticle(emitter, particleMotion, 0);
		time_ = 0;
	}
}

void Background::SpriteDraw() {
	sprite_->Draw();
}

void Background::Draw(const ViewProjection& viewProjection) {
	
}
