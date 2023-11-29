#pragma once

#include <memory>
#include <vector>

#include "Sprite.h"
#include "Random.h"
#include "Model.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

class TitleSprites {
public:
	TitleSprites();
	void Init();
	void Update();
	void FarDraw();
	void Draw(const ViewProjection& viewProjection);
	void NearDraw();

	void CreateParticle();
private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

private:
	enum SpriteOnOFF {
		kOn,
		kOff,

		kCount,
	};

	enum SpriteNames {
		kBackground,
		kTitle,
		kDecisionA,
		kSpriteCount,
	};

	std::string spriteNames_[kSpriteCount] = {
		"黒背景の",
		"タイトルの",
		"プッシュAの",
	};

	enum V2ItemNames {
		kPos,
		kV2ItemCount,
	};

	std::string v2ItemNames_[kV2ItemCount] = {
		"ポジション",
	};

	std::unique_ptr<Sprite> sprites_[kSpriteCount];

	Vector2 v2Info_[kSpriteCount][kV2ItemCount];

	std::string groupName_ = "titleSprites";

	uint32_t testTextureHandle_[SpriteOnOFF::kCount];

	std::vector<Model*> models_;
	std::vector<WorldTransform> worldTransforms_;
	WorldTransform worldTransform_;

	Random::RandomNumberGenerator rnd_;
	int32_t flashingCount_;
	bool on_;
	int32_t onMin_;
	int32_t onMax_;
	int32_t offMin_;
	int32_t offMax_;

	float time_;
	float kMaxTime_;
};

