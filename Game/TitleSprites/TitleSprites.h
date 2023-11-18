#pragma once

#include <vector>

#include "Sprite.h"
#include <memory>

class TitleSprites {
public:
	TitleSprites();
	void Init();
	void Update();
	void FarDraw();
	void NearDraw();

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

	enum SpriteNames {
		kBackground,
		kSelect,
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
};

