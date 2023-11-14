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
		for (size_t i = 0; i < emitter_->inOnce; i++) {
			//rnd_.NextFloatRange(particle->motion.velocity.speed - particle->motion.velocity.randomRange, particle->motion.velocity.speed + particle->motion.velocity.randomRange);

			// パーティクル
			ParticleWorldTransform* particle = new ParticleWorldTransform();
			particle->motion = *originalParticle_;
			float angle = rnd_.NextFloatRange(particle->motion.angle.start, particle->motion.angle.end);
			Vector3 vector = { std::cos(angle),std::sin(angle),0.0f };
			particle->motion.velocity.velocity = vector * rnd_.NextFloatRange(particle->motion.velocity.speed - particle->motion.velocity.randomRange, particle->motion.velocity.speed + particle->motion.velocity.randomRange);
			particle->motion.position = emitter_->position;
			particle->motion.aliveTime.time = rnd_.NextFloatRange(particle->motion.aliveTime.time - particle->motion.aliveTime.randomRange, particle->motion.aliveTime.time + particle->motion.aliveTime.randomRange);
			particle->motion.isAlive = true;
			// ワールドトランスフォーム
			particle->constantDate.color = particle->motion.color.currentColor;
			particle->scale = particle->motion.scale.currentScale;
			particle->rotate = particle->motion.rotate.currentRotate;
			particle->transform = particle->motion.position;
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
			if (particle->motion.aliveTime.time <= 0) {
				particle->motion.isAlive = false;
				isAlive_ = false;
			}
			else {
				// 更新
				particle->motion.position += particle->motion.velocity.velocity;
				particle->motion.aliveTime.time--;

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
	constantDate.matWorld = MakeIdentity4x4(); // 変形をリセット
	constantDate.matWorld *= matScale;          // ワールド行列にスケーリングを反映
	constantDate.matWorld *= matRot;            // ワールド行列に回転を反映
	constantDate.matWorld *= matTrans;          // ワールド行列に平行移動を反映
}
