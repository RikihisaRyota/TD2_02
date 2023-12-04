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
	void DrawBackground();

	const bool GetIsRetry() const { return isRetry_; }
	const bool GetTransition() const { return isTransition_; }
	const bool GetIsInitialize() const { return isInitialize_; }
	const bool GetIsStageSelect() const { return isSelect_; }
	void SetIsClear(const bool* is) { isClear_ = is; }
	void SetIsInitialize(bool flag) { isInitialize_ = flag; }
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

	bool isSelect_;

	bool isTransition_;

	bool isInitialize_;

	int separation_;

	std::unique_ptr<Sprite> background_;

	float time_;

	float kMaxTime_;

	int32_t cancelSoundHandle_;

	enum SpriteNames {
		kRetry,
		kStageSelect,
		kSpriteCount,
	};

	std::array<std::string, SpriteNames::kSpriteCount> spriteNames_ = {
		"リトライの",
		"ステージセレクトの",
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

