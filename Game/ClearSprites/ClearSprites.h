#pragma once

#include <vector>
#include <memory>

#include "ParticleShaderStruct.h"
#include "Input.h"
#include "Sprite.h"
#include "Random.h"

class ClearSprites {
public:
	enum State {
		kSelectStageState,
		kRetryState,
		kNextStageState,

		kStateCount,
	};
public:
	ClearSprites();
	void Init();
	void Update();
	void Draw();

	int GetState() { return state_; }
	bool GetAnimationFlag() { return isAnimation_; }
private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	void CreateParticle(const Vector2& position);
	void CreateCompleteParticle();
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
		kRetry,

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
		"ランク",
		"リトライ"
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
	uint32_t retry_[kFlagCount];

	Vector2 v2Info_[kSpriteCount][kV2ItemCount];

	std::string groupName_ = "clearSprites";

	Random::RandomNumberGenerator rnd_;

	int timePlace_[3];
	int conditionTimePlace_[3];
	int itemPlace_[2];
	int conditionItemPlace_[2];
	bool starFlag_[3];
	State state_;
	int currentStageNo_;

	bool isAnimation_;
	float animationCount_;
	float kMaxAnimationCount_;

	float firstStarCount_;
	float secondStarCount_;
	float thirdStarCount_;
	float kMaxStarCount_;

	Vector2 startStarSize_;
	Vector2 endStarSize_;

	bool createFlag_[3];

	Emitter* emitter_;
	ParticleMotion* particleMotion_;
	Vector2 position_;
	float speed_;
	float acceleration_;
};

