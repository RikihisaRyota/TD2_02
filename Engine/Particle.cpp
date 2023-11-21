#include "Particle.h"

#include "MyMath.h"

void Particle::Initialize(Emitter* emitter, ParticleMotion* particleMotion) {
	emitter_.reset(emitter);
	originalParticle_.reset(particleMotion);
	flameInterval_ = emitter->flameInterval;
	emitter->flameInterval = 0;
	isAlive_ = true;
}

void Particle::Update() {
	// 生成
	if (emitter_->aliveTime > 0) {
		if (emitter_->flameInterval <= 0) {
			for (size_t i = 0; i < emitter_->inOnce; i++) {
				// パーティクル
				ParticleWorldTransform* particle = new ParticleWorldTransform();
				particle->motion = *originalParticle_;
				float angle = 0.0f;
				if (emitter_->angle.start != emitter_->angle.end) {
					angle = rnd_.NextFloatRange(emitter_->angle.start, emitter_->angle.end);
				}
				Vector3 vector = { std::cos(angle),std::sin(angle),0.0f };
				particle->motion.velocity.velocity = vector * rnd_.NextFloatRange(particle->motion.velocity.speed - particle->motion.velocity.randomRange, particle->motion.velocity.speed + particle->motion.velocity.randomRange);
				particle->motion.position.x = rnd_.NextFloatRange(emitter_->spawn.position.x - emitter_->spawn.rangeX, emitter_->spawn.position.x + emitter_->spawn.rangeX);
				particle->motion.position.y = rnd_.NextFloatRange(emitter_->spawn.position.y - emitter_->spawn.rangeY, emitter_->spawn.position.y + emitter_->spawn.rangeY);
				particle->motion.aliveTime.time = rnd_.NextIntRange(particle->motion.aliveTime.time - particle->motion.aliveTime.randomRange, particle->motion.aliveTime.time + particle->motion.aliveTime.randomRange);
				particle->motion.aliveTime.maxTime = particle->motion.aliveTime.time;
				particle->motion.isAlive = true;
				// ワールドトランスフォーム
				particle->constantDate.color = particle->motion.color.currentColor;
				particle->scale = particle->motion.scale.currentScale;
				particle->rotate = particle->motion.rotate.currentRotate;
				particle->transform = particle->motion.position;
				particle->UpdateMatrix();
				particleWorldTransform_.emplace_back(std::move(particle));

			}
			emitter_->flameInterval = flameInterval_;
		}
		emitter_->flameInterval--;
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
				float t = 1.0f - float(particle->motion.aliveTime.time) / float(particle->motion.aliveTime.maxTime);
				// 更新
				// 色
				particle->motion.color.currentColor = Lerp(particle->motion.color.startColor, particle->motion.color.endColor, std::clamp(t, 0.0f, 1.0f));
				// スケール
				if (particle->motion.scale.interimScale == Vector3(0.0f, 0.0f, 0.0f)) {
					particle->motion.scale.currentScale = Lerp(particle->motion.scale.startScale, particle->motion.scale.endScale, std::clamp(t, 0.0f, 1.0f));
				}
				else {
					if (t <= 0.5f) {
						particle->motion.scale.currentScale = Lerp(particle->motion.scale.startScale, particle->motion.scale.interimScale, std::clamp(t, 0.0f, 0.5f));
					}
					else {
						particle->motion.scale.currentScale = Lerp(particle->motion.scale.interimScale, particle->motion.scale.endScale, std::clamp(t, 0.5f, 1.0f));
					}
				}
				// 回転
				particle->motion.rotate.currentRotate += particle->motion.rotate.addRotate;
				// 移動
				particle->motion.position += particle->motion.velocity.velocity;
				particle->motion.velocity.velocity += particle->motion.acceleration_;

				particle->scale = particle->motion.scale.currentScale;
				particle->rotate = particle->motion.rotate.currentRotate;
				particle->transform = particle->motion.position;
				particle->UpdateMatrix();
				particle->constantDate.color = particle->motion.color.currentColor;
				particle->motion.aliveTime.time--;
				numAliveParticle_++;
			}
		}
		else {
			break;
		}
	}
	// 死んでいたら消す
	particleWorldTransform_.erase(std::remove_if(
		particleWorldTransform_.begin(), particleWorldTransform_.end(),
		[](const std::unique_ptr<ParticleWorldTransform>& particle) {
			return !particle->motion.isAlive;
		}),
		particleWorldTransform_.end()
	);

	isAlive_ = false;
	for (auto& particle : particleWorldTransform_) {
		if (particle->motion.isAlive) {
			isAlive_ = true;
			break;
		}
	}
}

void Particle::Reset() {
	particleWorldTransform_.clear();
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
