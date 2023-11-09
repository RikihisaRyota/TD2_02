#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <optional>

class Player
{
public:

	// 状態
	enum class Status {
		kNormal, // 通常時
		

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
	void StatusRequest(Status status) { status_ = status; }

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

	enum FloatParameterNames {
		kMoveSpeed, // 移動スピード
		kJumpInitialVelocity, // ジャンプ時の初速
		kGravity, // 重力加速度
		kFallingGravity, // 降下中の重力加速

		kCountFloatParameter, // 末尾
	};

	// 定数パラメータ配列
	float parameters_[FloatParameterNames::kCountFloatParameter];

	std::string floatParameterItemNames[FloatParameterNames::kCountFloatParameter] = {
		"移動スピード", // 移動スピード
		"ジャンプ時の初速", // ジャンプ時の初速
		"重力加速度", // 重力加速度
		"降下中の重力加速", // 降下中の重力加速
	};

	// 今の状態
	Status status_ = Status::kNormal;

	// 状態のリクエスト
	std::optional<Status> statusRequest_ = std::nullopt;

	// グローバル変数のグループネーム
	const std::string groupName_ = "プレイヤー";

};
