#pragma once

#include <memory>

#include "PlaneRenderer.h"
#include "ViewProjection.h"
#include "WorldTransform.h"

#include "Collision/Collider.h"

class Player;
class Goal : public Collider
{
private:
	enum GoalNames {
		kStage_1,
		kStage_2,
		kStage_3,
		kStage_4,
		kStage_5,
		kStage_6,
		kStage_7,
		kStage_8,
		kGoalCount,
	};
	enum V2ItemNames {
		kPos,
		kScale,
		kV2ItemCount,
	};
public:
	Goal();
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

	WorldTransform* GetWorldTransform() { return &worldTransform_; }
private:
	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	void OnCollision() override;

	void SetCollider();

	std::string groupName_ = "Goal";

	std::string spriteNames_[kGoalCount] = {
		"ステージ_1",
		"ステージ_2",
		"ステージ_3",
		"ステージ_4",
		"ステージ_5",
		"ステージ_6",
		"ステージ_7",
		"ステージ_8",
	};

	std::string v2ItemNames_[kV2ItemCount] = {
		"ポジション",
		"スケール",
	};

	Vector3 v2Info_[kGoalCount][kV2ItemCount];

	WorldTransform worldTransform_;
	std::unique_ptr<PlaneRenderer> model_;
	uint32_t texture_;
};