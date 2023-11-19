#pragma once

#include <vector>

#include "Sprite.h"
#include <memory>

#include <optional>

class SelectSprites {
public:
	SelectSprites();
	void Init();
	void Update();
	void FarDraw();
	void NearDraw();

public:
	enum class State {
		kSelect,
		kMove,
	};

	void StateRequest(State state);

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	void SetStageNo();

	void SelectInitialize();

	void SelectUpdate();

	void MoveInitialize();

	void MoveUpdate();

	static void (SelectSprites::* spStateInitFuncTable[])();

	static void (SelectSprites::* spStateUpdateFuncTable[])();

private:

	State state_ = State::kSelect;
	std::optional<State> stateRequest_ = std::nullopt;

	bool isRight_;

	const int kMaxStage_ = 9;

	float kStageTexSize_;

	int nowStage_;

	int countFrame_;

	enum IItemNames {
		kMaxMoveFrame,
		kIItemCount,
	};

	std::string iItemNames_[IItemNames::kIItemCount] = {
		"移動フレーム"
	};

	int iInfo_[IItemNames::kIItemCount];

	enum SpriteNames {
		kSelect,
		kDecisionA,
		kSelectLS,
		kStarFram,
		kSpriteCount,
	};

	std::string spriteNames_[kSpriteCount] = {
		"セレクトの",
		"決定Aの",
		"選択の",
		"星のフレームの"
	};

	enum V2ItemNames {
		kPos,
		kV2ItemCount,
	};

	std::string v2ItemNames_[kV2ItemCount] = {
		"ポジション",
	};

	std::unique_ptr<Sprite> blackBackground_;

	std::unique_ptr<Sprite> sprites_[kSpriteCount];

	Vector2 v2Info_[kSpriteCount][kV2ItemCount];


	enum FItemNames {
		kStagePosY,
		kStageWidthInterval,
		kStarPosY,
		kStarWidthInterval,
		kStageSmallScale,
		kFItemCount,
	};

	std::string fItemNames_[FItemNames::kFItemCount] = {
		"ステージ枠のy座標",
		"ステージ枠の間隔",
		"評価スターのy座標",
		"評価スターの間隔",
		"ステージの小さい方のスケール",
	};

	float fInfo_[FItemNames::kFItemCount];

	uint32_t stageTeces_[10];
	std::unique_ptr<Sprite> stages_[5];

	enum StarState {
		kNotGet,
		kGet,
	};
	uint32_t starTeces_[2];
	std::unique_ptr<Sprite> stars_[3];

	Vector2 stageBasePos_;

	Vector2 stageMovePos_;

	Vector2 starBasePos_;

	std::string groupName_ = "selectSprites";
};

