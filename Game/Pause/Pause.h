#pragma once

#include "Sprite.h"
#include <memory>

class Pause {
public:
	Pause();
	void Init();
	void Update();
	void Draw();

	const bool GetIsPause() const { return isPause_; }
	void SetIsClear(const bool* is) { isClear_ = is; }

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

	const bool* isClear_ = nullptr;

	bool isPause_;

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

	std::string groupName_ = "Pause";

	uint32_t testTextureHandle_[SpriteOnOFF::kCount];
};

