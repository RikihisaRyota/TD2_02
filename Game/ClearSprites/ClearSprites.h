#pragma once

#include <vector>
#include <memory>

#include "Sprite.h"
#include "Random.h"

class ClearSprites {
public:
	ClearSprites();
	void Init();
	void Update();
	void Draw();

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
	enum Number {
		kZero,
		kOne,
		kTwo,
		kThree,
		kFore,
		kFive,
		kSix,
		kSeven,
		kEight,
		kNine,
		kNumberCount,
	};

	enum Star {
		kTrue,
		kFalse,

		kStarCount,
	};

	enum SpriteNames {
		kBackground,
		kResult,
		kTime,
		kTimeOnesPlace,
		kTimeTensPlace,
		kTimeHundredsPlace,
		kStarFirst,
		kStarSecond,
		kStarThird,
		kConditionFirst,
		kConditionSecond,
		kConditionThird,
		kSelectStage,
		kNextStage,
		kDecisionA,
		kSelectLS,
		kRank,

		kSpriteCount,
	};

	std::string spriteNames_[kSpriteCount] = {
		"黒背景の",
		"リザルト",
		"タイム",
		"一の位",
		"十の位",
		"百の位",
		"星1",
		"星2",
		"星3",
		"条件1",
		"条件2",
		"条件3",
		"セレクトステージ",
		"次のステージ",
		"Aボタン",
		"Lスティック"
	};

	enum V2ItemNames {
		kPos,
		kScale,
		kV2ItemCount,
	};

	std::string v2ItemNames_[kV2ItemCount] = {
		"ポジション",
		"スケール",
	};

	std::unique_ptr<Sprite> sprites_[kSpriteCount];
	uint32_t number_[kNumberCount];
	uint32_t star_[kStarCount];

	Vector2 v2Info_[kSpriteCount][kV2ItemCount];

	std::string groupName_ = "clearSprites";

	Random::RandomNumberGenerator rnd_;

	int timePlace_[3];
	bool starFlag_[3];
};

