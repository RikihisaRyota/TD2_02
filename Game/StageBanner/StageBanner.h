#pragma once

#include <memory>

#include "Sprite.h"
#include "MapChip.h"

class StageBanner {
public:
	StageBanner();
	void Initialize();
	void Update();
	void Draw();

	const bool GetIsAnimation() const { return isAnimation_; }
private:
	uint32_t stageTextureHandle_[MapChip::Stage::kCount];
	uint32_t frameTextureHandle_;
	std::unique_ptr<Sprite> stageSprite_;
	std::unique_ptr<Sprite> frameSprite_;
	Vector2 position_;
	Vector2 startPosition_;
	Vector2 endPosition_;

	Vector2 stageSize_;
	Vector2 frameSize_;

	bool isAnimation_;
	float animationCount_;
	float kMaxAnimationCount_;
};