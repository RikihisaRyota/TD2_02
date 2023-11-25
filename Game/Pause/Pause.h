#pragma once

#include "Sprite.h"
#include <memory>
#include <array>

class Pause {
public:
	Pause();
	void Init();
	void Update();
	void Draw();

	const bool GetIsRetry() const { return isRetry_; }
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

	bool isRetry_;


	enum SpriteNames {
		kRetry,
		kSpriteCount,
	};

	std::array<std::string, SpriteNames::kSpriteCount> spriteNames_ = {
		"リトライの",
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

	std::string groupName_ = "Pause";
};

