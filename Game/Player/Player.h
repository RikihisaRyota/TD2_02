#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <optional>

#include "Collision/Collider.h"

class Player : public Collider
{
public:

	// 状態
	enum class Status {
		kNormal, // 通常時
		kJump, // ジャンプ時
		kGripWall, // 壁に張り付いている時
		kWallJump, 
	};

	Player();
	~Player() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 3DモデルのDraw仮
	/// </summary>
	void Draw(const ViewProjection& viewProjection);

public:

	/// <summary>
	/// 状態のリクエスト
	/// </summary>
	/// <param name="status">したい状態</param>
	void StatusRequest(Status status) { statusRequest_ = status; }

	/// <summary>
	/// 今の状態の確認。あたり判定のフラグに使用。
	/// </summary>
	/// <returns>状態</returns>
	Status GetStatus() { return status_; }

	/// <summary>
	/// 速度の参照。あたり判定用。
	/// </summary>
	/// <returns>速度</returns>
	Vector3* GetVelocity() { return &velocity_; }
	//const Vector3& GetVelocity() { return velocity_; }

	WorldTransform* GetWorldTransform() { return &worldTransform_; }

	void UpdateMatrix();


private:

	void OnCollision() override;

	void SetCollider();

	/// <summary>
	/// jsonファイルへの書き込み
	/// </summary>
	void SetGlobalVariable();

	/// <summary>
	/// jsonファイルからの呼び出し
	/// </summary>
	void ApplyGlobalVariable();

	/// <summary>
	/// 通常の初期化
	/// </summary>
	void NormalInitialize();
	/// <summary>
	/// 通常のアップデート
	/// </summary>
	void NormalUpdate();

	void JumpInitialize();

	void JumpUpdate();

	void GripWallInitialize();

	void GripWallUpdate();

	void WallJumpInitialize();

	void WallJumpUpdate();
	
private:

	// モデルのパーツ
	enum Parts {
		kMain, // 胴体 
		kCountParts, // 末尾
	};
	
	// ワールドトランスフォーム。一番の親。
	WorldTransform worldTransform_;
	// モデル配列
	std::vector<std::unique_ptr<Model>> models_;
	// モデルのワールドトランスフォーム配列
	std::vector<WorldTransform> modelWorldTransforms_;

	// 速度
	Vector3 velocity_;
	// ジャンプ中か
	bool isJump_;
	// 右向きか
	bool isRight_;

	int countFrame_;

	enum FloatParameterNames {
		kMoveSpeed, // 移動スピード
		kJumpInitialVelocity, // ジャンプ時の初速
		kGravity, // 重力加速度
		kFallingGravity, // 降下中の重力加速
		kJumpAccelerationX, // ジャンプの横の加速度
		kJumpMaxSpeedX, // ジャンプのx軸の最大の速度
		kCountFloatParameter, // 末尾
	};

	// 定数パラメータ配列
	float parameters_[FloatParameterNames::kCountFloatParameter];

	std::string floatParameterItemNames[FloatParameterNames::kCountFloatParameter] = {
		"移動スピード", // 移動スピード
		"ジャンプ時の初速", // ジャンプ時の初速
		"重力加速度", // 重力加速度
		"降下中の重力加速", // 降下中の重力加速
		"ジャンプのx軸の加速度",
		"ジャンプのx軸の最大の速度",
	};

	enum V3ParameterNames {
		kInitialPos, // 初期座標
		kCountV3Parameter, // 末尾
	};

	// 定数パラメータ配列
	Vector3 v3Parameters_[V3ParameterNames::kCountV3Parameter];

	std::string v3ParameterItemNames[V3ParameterNames::kCountV3Parameter] = {
		"初期座標",
	};

	enum IParameterNames {
		kGripStayTime, // 壁に捕まって動かないフレーム数
		kCountIParameter, // 末尾
	};

	// 定数パラメータ配列
	int iParameters_[IParameterNames::kCountIParameter];

	std::string iParameterItemNames[IParameterNames::kCountIParameter] = {
		"壁に捕まって動かないフレーム数",
	};

	Vector3 preInitialPos_;

	// 今の状態
	Status status_ = Status::kNormal;

	// 状態のリクエスト
	std::optional<Status> statusRequest_ = std::nullopt;

	// グローバル変数のグループネーム
	const std::string groupName_ = "Player";

};
