#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <optional>

#include "Random.h"
#include "Collision/Collider.h"
#include "PlaneRenderer.h"
#include "ParticleShaderStruct.h"
class Player : public Collider
{
public:

	// 状態
	enum class State {
		kNormal, // 通常時
		kJump, // ジャンプ時
		kGripWall, // 壁に張り付いている時
		kWallJump, 
		kWallSideJump,
		kWallDownJump,
		kClearMove,
		kDeadMove,
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

	void DrawUI(const ViewProjection& viewProjection);

public:

	/// <summary>
	/// 状態のリクエスト
	/// </summary>
	/// <param name="status">したい状態</param>
	void StateRequest(State state);

	/// <summary>
	/// 今の状態の確認。あたり判定のフラグに使用。
	/// </summary>
	/// <returns>状態</returns>
	State GetStatus() { return state_; }

	/// <summary>
	/// 速度の参照。あたり判定用。
	/// </summary>
	/// <returns>速度</returns>
	Vector3* GetVelocity() { return &velocity_; }
	//const Vector3& GetVelocity() { return velocity_; }

	WorldTransform* GetWorldTransform() { return &worldTransform_; }

	void UpdateMatrix();

	const bool GetIsChangeCamera() const { return isChangeCamera_; }

	bool GetIsClear() { return isClear_; }
private:

	void OnCollision() override;

	void SetCollider();

	void NoTatchUpdate();

	void NoTatchReturnUpdate();

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

	void WallSideJumpInitialize();

	void WallSideJumpUpdate();

	void WallDownJumpInitialize();

	void WallDownJumpUpdate();

	void ClearMoveInitialize();

	void ClearMoveUpdate();

	void DeadModeInitialize();

	void DeadModeUpdate();

	void FloarAndWallInit();

	void FloarAndWallUpdate();

	static void (Player::* spStateInitFuncTable[])();
	
	static void (Player::* spStateUpdateFuncTable[])();


	// 俺が追加した
	// プレイヤーの後ろにパーティクルを追加
	void ParticleInitialize();
	void ParticleUpdate();
	void ParticleCreate(const Vector2& vector);

	void SoundInitialize();
private:

	// モデルのパーツ
	enum Parts {
		kMain, // 胴体 
		kTail, // しっぽ
		kCountParts, // 末尾
	};
	
	// ワールドトランスフォーム。一番の親。
	WorldTransform worldTransform_;
	// モデル配列
	std::vector<Model*> models_;
	std::unique_ptr<PlaneRenderer>face_;
	WorldTransform faceWorldTransform_;
	// モデルのワールドトランスフォーム配列
	std::vector<WorldTransform> modelWorldTransforms_;

	// 顔のテクスチャハンドル
	uint32_t faceTextureHandle_[2];
	// 速度
	Vector3 velocity_;
	// ジャンプ中か
	bool isJump_;
	// 右向きか
	bool isRight_;
	// ２段ジャンプか
	int jumpCount_;
	bool isPlayerFaceRight_;

	bool kIs2Jump_ = true;

	bool kIs2WallJump_ = true;

	bool kIsWallDown_ = false;

	bool isChangeCamera_;

	Vector2 goalPos_;
	Vector2 preClearPos_;
	Vector3 preClearScale_;
	Vector3 preScale_;

	float scaleTheta_;

	int noTatchCountFrame_;

	float clearRot_;

	int countFrame_;

	enum FloatParameterNames {
		kMoveSpeed, // 移動スピード
		kJumpInitialVelocity, // ジャンプ時の初速
		kGravity, // 重力加速度
		kFallingGravity, // 降下中の重力加速
		kJumpAccelerationX, // ジャンプの横の加速度
		kJumpMaxSpeedX, // ジャンプのx軸の最大の速度
		kWallJumpInitialVelocityX, // 壁キック時のx軸の初速
		kWallJumpInitialVelocityY, // 壁キック時のy軸の初速
		kJumpRotateSpeed, // ジャンプ時のプレイヤーの回転スピード
		k2JumpMagnification, // 2段ジャンプの倍率
		kClearRotateSpeed, // クリア時の回転の速さ
		kWallGravity,
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
		"壁キック時のx軸の初速",
		"壁キック時のy軸の初速",
		"ジャンプ時のプレイヤーの回転スピード",
		"2段ジャンプの倍率",
		"クリア時の回転の速さ",
		"壁ずり時の落下加速度"
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

	//enum V2ParameterNames {
	//	kCountV2Parameter, // 末尾
	//};

	//// 定数パラメータ配列
	//Vector2 v2Parameters_[V2ParameterNames::kCountV2Parameter];

	//std::string v2ParameterItemNames[V2ParameterNames::kCountV2Parameter] = {
	//	
	//};

	enum IParameterNames {
		kGripStayTime, // 壁に捕まって動かないフレーム数
		k2JumpExtensionFrame, // 2段ジャンプの猶予フレーム
		kClearFrame, // クリア時にかけるフレーム数
		kCountIParameter, // 末尾
	};

	// 定数パラメータ配列
	int iParameters_[IParameterNames::kCountIParameter];

	std::string iParameterItemNames[IParameterNames::kCountIParameter] = {
		"壁に捕まって動かないフレーム数",
		"2段ジャンプの猶予フレーム",
		"クリア時にかけるフレーム数",

	};

	Vector3 preInitialPos_;

	// 今の状態
	State state_ = State::kNormal;

	State preState_ = State::kNormal;

	// 状態のリクエスト
	std::optional<State> stateRequest_ = std::nullopt;

	// グローバル変数のグループネーム
	const std::string groupName_ = "Player";

	// 所有権はパーティクルマネージャーが持っている
	Emitter* emitter_;
	ParticleMotion* particleMotion_;
	bool isCreateParticle_;
	Random::RandomNumberGenerator rnd_;

	// 音関係
	int32_t jumpSoundHandle_;
	int32_t deathSoundHandle_;

	// クリアフラグ
	bool isClear_;
	int time_;
	int itemCount_;
};
