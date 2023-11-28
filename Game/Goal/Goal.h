#pragma once

#include <memory>

#include "PlaneRenderer.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Collision/Collider.h"
#include "ParticleShaderStruct.h"
#include "Random.h"
#include "MapChip.h"

class Player;
class Goal : public Collider
{
private:
	enum V2ItemNames {
		kPos,
		kScale,
		kV2ItemCount,
	};
public:
	Goal();
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	WorldTransform* GetWorldTransform() { return &worldTransform_; }
private:
	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	void OnCollision() override;

	void SetCollider();

	void ParticleInitialize();
	void CreateParticle();

	std::string groupName_ = "Goal";

	std::string spriteNames_[MapChip::Stage::kCount] = {
		"ステージ_1",
		"ステージ_2",
		"ステージ_3",
		"ステージ_4",
		"ステージ_5",
		"ステージ_6",
		"ステージ_7",
		"ステージ_8",
		"ステージ_9",
		"ステージ_10",
		"ステージ_11",
		"ステージ_12",
		"ステージ_13",
	};

	std::string v2ItemNames_[kV2ItemCount] = {
		"ポジション",
		"スケール",
	};

	Vector3 v2Info_[MapChip::Stage::kCount][kV2ItemCount];

	WorldTransform worldTransform_;
	std::unique_ptr<PlaneRenderer> model_;
	uint32_t goalTexture_;

	static const int kNumParticle = 6;
	Emitter* emitter_[kNumParticle];
	ParticleMotion* particleMotion_[kNumParticle];
	float angle_;
	float addAngle_;
	float aliveTime_;
	float speed_;
	float colorTime_;
	uint32_t particleTexture_;
	Random::RandomNumberGenerator rnd_;
};