#pragma once

#include "PlaneRenderer.h"
#include <memory>
#include <array>
#include "WorldTransform.h"
#include "ViewProjection.h"

class StageUI {
public:
	StageUI();
	void Init();
	void Update();
	void Draw(const ViewProjection& viewProjection);

private:

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	void SetPos();

private:

	int countFrame_;

	enum TexType {
		kEmpty,
		kRight,
		kCountTexType,
	};

	std::array<uint32_t, TexType::kCountTexType> aTeces_;

	enum SpriteNames {
		kJumpUi,
		kLStickLeft,
		kLStickRight,
		kWallJumpLeft,
		kWallJumpRight,
		kSpriteCount,
	};

	std::array<std::string, SpriteNames::kSpriteCount> spriteNames_ = {
		"Aボタンの",
		"Lスティック左の",
		"Lスティック右の",
		"左壁キックの",
		"右壁キックの",
	};

	enum V2ItemNames {
		kPos,
		kV2ItemCount,
	};

	std::array<std::string, V2ItemNames::kV2ItemCount> v2ItemNames_ = {
		"ポジション",
	};

	std::array<std::unique_ptr<PlaneRenderer>, SpriteNames::kSpriteCount> models_;

	std::array<uint32_t, SpriteNames::kSpriteCount> teces_;

	std::array<bool, SpriteNames::kSpriteCount> isDraws_;

	std::array<WorldTransform, SpriteNames::kSpriteCount> worldTransforms_;

	std::array<std::array<Vector2, V2ItemNames::kV2ItemCount>, SpriteNames::kSpriteCount> v2Info_;

	enum FInfoNames {
		kNearZ,
		kFarZ,
		kScale,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"壁キックUiのz座標",
		"移動とジャンプUiのz座標",
		"スケール"
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

	std::string groupName_ = "StageUI";


};

