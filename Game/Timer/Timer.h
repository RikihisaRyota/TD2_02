#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include "Vector3.h"
#include <list>
#include "Model.h"
#include <array>
#include <optional>
#include "Sprite.h"

class Timer
{
public:
	Timer();

	void Init();

	void Update();

	void DrawUI();

	void SetIsClear(const bool* is) { isClear_ = is; }

	void SetStageNo(const int* no) { stage_ = no; }

	const int GetTime() const { return second_ * 60; }

private:

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void SetNumTeces();

	void SetSpriteSize();

private:

	int time_;

	int second_;

	const int* stage_;

	const bool* isClear_ = nullptr;

	Vector2 timerSize_;
	Vector2 numSize_;
	Vector2 targetSize_;

	static const int MaxDigits = 2;

	enum SpriteNames {
		kTimerSprite,
		kTargetTime,
		kSpriteCount,
	};

	std::string spriteNames_[kSpriteCount] = {
		"タイマーの",
		"目標タイム"
	};

	enum V2ItemNames {
		kPos,
		kV2ItemCount,
	};

	std::array<std::string, V2ItemNames::kV2ItemCount> v2ItemNames_ = {
		"ポジション",
	};

	std::array<std::unique_ptr<Sprite>, SpriteNames::kSpriteCount> sprites_;

	std::array<std::array<Vector2, V2ItemNames::kV2ItemCount>, SpriteNames::kSpriteCount> v2Info_;

	enum TexColor {
		kBright,
		kDark,
		kColorCount,
	};

	enum DrawNumType {
		kTimer,
		kNumTypeCount,
	};

	std::array<std::array<std::unique_ptr<Sprite>, MaxDigits>, DrawNumType::kNumTypeCount> numSprites_;

	std::string numItemNames[DrawNumType::kNumTypeCount] = {
		"数字の",
	};

	std::array<Vector2, DrawNumType::kNumTypeCount> numPoses_;

	std::array<std::array<uint32_t, 10>, TexColor::kColorCount> numTeces_;

	const std::string groupName_ = "Timer";

	const std::string groupName2_ = "selectSprites";

	enum FInfoNames {
		kNumericInterval,
		kTimerScale,
		kNumScale,
		kTargetScale,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"数字の間隔",
		"タイマーのスケール",
		"数字のスケール",
		"目標タイムのスケール"
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;
};
