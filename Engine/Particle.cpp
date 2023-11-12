#include "Particle.h"

#include "MyMath.h"

Particle::~Particle() {
	delete originalParticle_;
	delete emitter_;
}

void Particle::Initialize(Emitter* emitter, ParticleMotion* particleMotion) {
	emitter_ = emitter;
	originalParticle_ = particleMotion;
	isAlive_ = true;
}

void Particle::Update() {
	// 生成
	if (emitter_->aliveTime > 0) {
		for (size_t i = 0; i < emitter_->createParticle; i++) {
			// パーティクル
			ParticleWorldTransform* particle = new ParticleWorldTransform();
			particle->motion = *originalParticle_;
			particle->motion.position = emitter_->position;
			particle->color = emitter_->color;
			particle->motion.isAlive = true;
			// ワールドトランスフォーム
			particle->scale = emitter_->scale;
			particle->rotate = emitter_->rotate;
			particle->transform = emitter_->position;
			particle->UpdateMatrix();
			particleWorldTransform_.emplace_back(std::move(particle));

		}
		emitter_->aliveTime--;
	}
	else {
		emitter_->isAlive = false;
	}
	numAliveParticle_ = 0;
	// 更新
	for (auto& particle : particleWorldTransform_) {
		// 生きてるかどうか
		if (particle->motion.isAlive) {
			if (particle->motion.aliveTime <= 0) {
				particle->motion.isAlive = false;
				isAlive_ = false;
			}
			else {
				// 更新
				particle->motion.position += particle->motion.velocity;
				particle->motion.velocity += particle->motion.acceleration;
				particle->motion.aliveTime--;

				particle->transform = particle->motion.position;
				particle->UpdateMatrix();
				numAliveParticle_++;
				isAlive_ = true;
			}
		}
	}

}

void Particle::ParticleWorldTransform::UpdateMatrix() {
	Matrix4x4 matScale, matRot, matTrans;
	matScale = MakeIdentity4x4();
	matRot = MakeIdentity4x4();
	matTrans = MakeIdentity4x4();

	// スケール、回転、平行移動行列の計算
	matScale = MakeScaleMatrix(scale);
	matRot *= MakeRotateXYZMatrix(rotate);
	matTrans = MakeTranslateMatrix(transform);

	// ワールド行列の合成
	matWorld = MakeIdentity4x4(); // 変形をリセット
	matWorld *= matScale;          // ワールド行列にスケーリングを反映
	matWorld *= matRot;            // ワールド行列に回転を反映
	matWorld *= matTrans;          // ワールド行列に平行移動を反映
}
