#pragma once

#include <vector>
#include <memory>

#include "Input.h"
#include "Sprite.h"
#include "Random.h"

class ClearSprites {
public:
	ClearSprites();
	void Init();
	void Update();
	void Draw();

	bool GetNextScene() { return nextStageFlag_; }
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

	enum Flag {
		kTrue,
		kFalse,

		kFlagCount,
	};

	enum SpriteNames {
		kBackground,
		kResult,
		kGoal,
		kItem,
		kTime,
		kClear,
		kTimeOnesPlace,
		kTimeTensPlace,
		kTimeHundredsPlace,
		kTimeConditionOnesPlace,
		kTimeConditionTensPlace,
		kTimeConditionHundredsPlace,
		kItemOnesPlace,
		kItemTensPlace,
		kItemConditionOnesPlace,
		kItemConditionTensPlace,
		kSlash0,
		kSlash1,
		kStarFirst,
		kStarSecond,
		kStarThird,
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
		"ゴール",
		"クリアフラグ",
		"アイテム",
		"タイム",
		"クリアタイム一の位",
		"クリアタイム十の位",
		"クリアタイム百の位",
		"クリア条件タイム一の位",
		"クリア条件タイム十の位",
		"クリア条件タイム百の位",
		"クリアアイテム一の位",
		"クリアアイテム十の位",
		"クリア条件アイテム一の位",
		"クリア条件アイテム十の位",
		"スラッシュ0",
		"スラッシュ1",
		"星1",
		"星2",
		"星3",
		"セレクトステージ",
		"次のステージ",
		"Aボタン",
		"Lスティック",
		"ランク"
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

	Input* input_;

	std::unique_ptr<Sprite> sprites_[kSpriteCount];
	uint32_t number_[kNumberCount];
	uint32_t conditionNumber_[kNumberCount];
	uint32_t star_[kFlagCount];
	uint32_t nextStage_[kFlagCount];
	uint32_t selectStage_[kFlagCount];

	Vector2 v2Info_[kSpriteCount][kV2ItemCount];

	std::string groupName_ = "clearSprites";

	Random::RandomNumberGenerator rnd_;

	int timePlace_[3];
	int conditionTimePlace_[3];
	int itemPlace_[2];
	int conditionItemPlace_[2];
	bool starFlag_[3];
	bool nextStageFlag_;
};

