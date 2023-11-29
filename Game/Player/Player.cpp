#include "Player.h"

#include "Audio.h"
#include "Input.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
// 俺が追加した
#include "Game/StageData/StageData.h"
#include "SceneSystem/IScene/IScene.h"
#include "ParticleManager.h"
#include "MyMath.h"

#include "Ease/Ease.h"
#include <numbers>
#include "MapChip.h"

Player::Player() {

	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::RIGID_BODY);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionAttributeGoal);
	SetCollisionMask(kCollisionAttributeOut);

	models_.emplace_back((ModelManager::GetInstance()->GetModel("player")));
	models_.emplace_back((ModelManager::GetInstance()->GetModel("playerTail")));

	for (int i = 0; i < Parts::kCountParts; i++) {
		modelWorldTransforms_.push_back(WorldTransform());
		modelWorldTransforms_[i].Initialize();
	}

	worldTransform_.Initialize();
	worldTransform_.translate_ = { 50.0f,20.0f,0.0f };
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	modelWorldTransforms_[Parts::kMain].parent_ = &worldTransform_;
	modelWorldTransforms_[Parts::kTail].parent_ = &worldTransform_;
	modelWorldTransforms_[Parts::kTail].translate_ = { -0.9f,-0.1f,0.0f };
	modelWorldTransforms_[Parts::kTail].rotation_.z = 0.5f;
	UpdateMatrix();

	isJump_ = true;
	isRight_ = true;
	isClear_ = false;
	velocity_ = {};
	parameters_[FloatParameterNames::kMoveSpeed] = 0.1f;
	parameters_[FloatParameterNames::kJumpInitialVelocity] = 1.0f;
	parameters_[FloatParameterNames::kGravity] = -0.02f;
	parameters_[FloatParameterNames::kFallingGravity] = -0.01f;

	v3Parameters_[V3ParameterNames::kInitialPos] = { 50.0f,20.0f,0.0f };
	preInitialPos_ = v3Parameters_[V3ParameterNames::kInitialPos];

	SetGlobalVariable();

	// パーティクル初期化
	particleTextureHandle_ = TextureManager::Load("Resources/Textures/particle.png");
	ParticleInitialize();

	// 音初期化
	SoundInitialize();
}

void Player::Initialize() {

	shapeType_->SetColliderType(BaseColliderShapeType::ColliderType::RIGID_BODY);

	StateRequest(State::kNormal);

	worldTransform_.translate_ = v3Parameters_[kInitialPos];
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.rotation_ = {};

	modelWorldTransforms_.at(Parts::kMain).scale_ = { 1.0f,1.0f,1.0f };
	modelWorldTransforms_.at(Parts::kMain).rotation_ = { 0.0f,0.0f,0.0f };
	modelWorldTransforms_.at(Parts::kMain).translate_ = { 0.0f,0.0f,0.0f };

	modelWorldTransforms_.at(Parts::kTail).scale_ = { 1.0f,1.0f,1.0f };
	modelWorldTransforms_.at(Parts::kTail).rotation_ = { 0.0f,0.0f,0.5f };
	modelWorldTransforms_.at(Parts::kTail).translate_ = { -0.9f,-0.1f,0.0 };

	UpdateMatrix();

	jumpCount_ = 0;
	isJump_ = true;
	isRight_ = true;
	velocity_ = {};
	velocity_.y = -0.001f;

	isCollisionGoal_ = false;
	isDead_ = false;
	isChangeCamera_ = false;
	isClear_ = false;
	clearTime_ = 0;

	/*if (IScene::stageNo_ == 2 || IScene::stageNo_ == 6 || IScene::stageNo_ == 11) {
		isAuto_ = true;
	}
	else {
		isAuto_ = false;
	}*/
	isAuto_ = false;
	isReservationJump_ = false;

	ParticleInitialize();
}

void Player::UpdateMatrix() {
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < Parts::kCountParts; i++) {
		modelWorldTransforms_[i].UpdateMatrix();
	}
}

void Player::ChangeStateGrip2Normal(const MapChip& mapChip)
{
	if (state_ == State::kGripWall && !stateRequest_) {
		if (isRight_) {

			int y = kMaxHeightBlockNum - static_cast<int>((worldTransform_.worldPos_.y + worldTransform_.scale_.y) / (kBlockSize));
			int x = static_cast<int>((worldTransform_.worldPos_.x + worldTransform_.scale_.x * 2) / (kBlockSize));

			if (MapChip::UseBlocks::kNone == mapChip.GetBlocksType(x, y) && MapChip::UseBlocks::kNone == mapChip.GetBlocksType(x, y + 1)) {
				StateRequest(State::kNormal);
			}

		}
		else {

			int y = kMaxHeightBlockNum - static_cast<int>((worldTransform_.worldPos_.y + worldTransform_.scale_.y) / (kBlockSize));
			int x = static_cast<int>((worldTransform_.worldPos_.x - worldTransform_.scale_.x * 2) / (kBlockSize));

			if (MapChip::UseBlocks::kNone == mapChip.GetBlocksType(x, y) && MapChip::UseBlocks::kNone == mapChip.GetBlocksType(x, y + 1)) {
				StateRequest(State::kNormal);
			}
		}
	}

}

void Player::OnCollision() {

	if (editInfo_.colliderTypeMask_ == BaseColliderShapeType::ColliderType::RIGID_BODY) {
		if ((velocity_.x != 0 && editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x == 0 &&
			editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y == 0) ||
			(velocity_.x == 0 && state_ == State::kFloarAndWall)) {
			StateRequest(State::kFloarAndWall);
			WallParticleCreate({ 0.0f,-1.0f });
		}
		else if (editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y == 0) {
			StateRequest(State::kNormal);
			WallParticleCreate({ 0.0f,-1.0f });
		}
		else if (velocity_.x != 0 && editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x == 0) {
			StateRequest(State::kGripWall);
			if (velocity_.x > 0) {
				isRight_ = true;
				WallParticleCreate({ 1.0f,0.0f });
			}
			else if (velocity_.x < 0) {
				isRight_ = false;
				WallParticleCreate({ -1.0f,0.0f });
			}
		}
		else {
			StateRequest(State::kNormal);
			WallParticleCreate({ 0.0f,1.0f });
		}

		worldTransform_.translate_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].x;
		worldTransform_.translate_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].y;

		velocity_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x;
		velocity_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y;

		UpdateMatrix();

		if ((editInfo_.collisionMask_ & kCollisionAttributeBlock) >= 0b1) {

			for (uint32_t no : editInfo_.i32Info_) {

				if (no == uint32_t(MapChip::UseBlocks::kRedBlock)) {
					shapeType_->SetColliderType(BaseColliderShapeType::ColliderType::UNKNOWN);
					StateRequest(State::kDeadMove);
					return;
				}
				else if (no == uint32_t(MapChip::UseBlocks::kNeedleBlock)) {

				}
			}
		}


	}
	else if (editInfo_.colliderTypeMask_ == BaseColliderShapeType::ColliderType::COLLIDER) {
		if ((editInfo_.collisionMask_ & kCollisionAttributeOut) >= 0b1) {
			shapeType_->SetColliderType(BaseColliderShapeType::ColliderType::UNKNOWN);
			StateRequest(State::kDeadMove);
			return;
		}
		else if ((editInfo_.collisionMask_ & kCollisionAttributeGoal) >= 0b1) {
			shapeType_->SetColliderType(BaseColliderShapeType::ColliderType::UNKNOWN);
			goalPos_ = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2MASKPOS];
			StateRequest(State::kClearMove);
			return;
		}
	}

}

void Player::SetCollider() {
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ worldTransform_.scale_.x,worldTransform_.scale_.y - 0.1f }, Vector2{ velocity_.x,velocity_.y });

	CollisionManager::GetInstance()->SetCollider(this);
}

void Player::NoTatchUpdate() {
	const uint16_t cycle = 200;

	const float pi = std::numbers::pi_v<float>;

	const float step = 2.0f * pi / cycle;

	scaleTheta_ += step;

	scaleTheta_ = std::fmod(scaleTheta_, 2.0f * pi);

	const float amplitude = 0.2f;

	float scale = std::sinf(scaleTheta_) * amplitude;

	worldTransform_.scale_ = { 1.0f - scale,1.0f + scale,1.0f };

}

void Player::NoTatchReturnUpdate() {
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
}

void Player::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		globalVariables->AddItem(groupName_, floatParameterItemNames[i], parameters_[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		globalVariables->AddItem(groupName_, v3ParameterItemNames[i], v3Parameters_[i]);
	}

	for (int i = 0; i < IParameterNames::kCountIParameter; i++) {
		globalVariables->AddItem(groupName_, iParameterItemNames[i], iParameters_[i]);
	}

	ApplyGlobalVariable();
}

void Player::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < FloatParameterNames::kCountFloatParameter; i++) {
		parameters_[i] = globalVariables->GetFloatValue(groupName_, floatParameterItemNames[i]);
	}

	for (int i = 0; i < V3ParameterNames::kCountV3Parameter; i++) {
		v3Parameters_[i] = globalVariables->GetVector3Value(groupName_, v3ParameterItemNames[i]);
	}

	for (int i = 0; i < IParameterNames::kCountIParameter; i++) {
		iParameters_[i] = globalVariables->GetIntValue(groupName_, iParameterItemNames[i]);
	}

	if (preInitialPos_ != v3Parameters_[V3ParameterNames::kInitialPos]) {
		preInitialPos_ = v3Parameters_[V3ParameterNames::kInitialPos];
		worldTransform_.translate_ = v3Parameters_[V3ParameterNames::kInitialPos];
	}
}

void Player::MoveInit() {
	if (isRight_) {
		worldTransform_.rotation_.y = 0.0f;
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	}
	rotateTheta_ = 0.0f;
}

void Player::MoveUpdate() {
	const uint16_t cycle = 20;

	const float pi = std::numbers::pi_v<float>;

	const float step = 2.0f * pi / cycle;

	rotateTheta_ += step;

	rotateTheta_ = std::fmod(rotateTheta_, 2.0f * pi);

	const float amplitude = 0.2f;

	float rotate = std::sinf(rotateTheta_) * amplitude;

	if (isRight_) {
		worldTransform_.rotation_.y = rotate;
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float> +rotate;
	}
}

void Player::NormalInitialize() {
	worldTransform_.rotation_ = {};
	countFrame_ = 0;
	if (preState_ != State::kJump) {
		jumpCount_ = 0;
	}
	noTatchCountFrame_ = 0;

	MoveInit();
}

void Player::NormalUpdate() {
	countFrame_++;

	Input* input = Input::GetInstance();

	if (velocity_.y == 0.0f) {
		isJump_ = false;
	}
	else {
		isJump_ = true;
	}

	Vector3 move = { input->GetGamePadLStick().x,0.0f,0.0f };

	if (move.x > 0) {
		isRight_ = true;
	}
	else if (move.x < 0) {
		isRight_ = false;
	}

	/*if (move.x == 0.0f) {
		noTatchCountFrame_++;

		if (noTatchCountFrame_ >= 120) {
			if (noTatchCountFrame_ == 120) {
				scaleTheta_ = 0.0f;
			}
			NoTatchUpdate();
		}
	}
	else {
		NoTatchReturnUpdate();
		noTatchCountFrame_ = 0;
	}*/

	move.x *= parameters_[FloatParameterNames::kMoveSpeed];

	if ((input->PressedGamePadButton(Input::GamePadButton::A) || isAuto_ || isReservationJump_) && !isJump_) {
		auto playHandle = Audio::GetInstance()->SoundPlayWave(jumpSoundHandle_);
		Audio::GetInstance()->SetValume(playHandle, 1.0f);
		if (jumpCount_ >= 1) {
			Audio::GetInstance()->SetPitch(playHandle, 1.5f);
		}
		else {
			Audio::GetInstance()->SetPitch(playHandle, 1.0f);
		}
		/*isJump_ = true;
		move.y += parameters_[FloatParameterNames::kJumpInitialVelocity];*/
		StateRequest(State::kJump);
	}
	else if (velocity_.y <= 0.0f) {
		move.y += parameters_[FloatParameterNames::kFallingGravity];
	}
	else {
		move.y += parameters_[FloatParameterNames::kGravity];
	}

	/*if (isRight_) {
		worldTransform_.rotation_.y = 0.0f;
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	}*/

	if (move.x != 0) {
		MoveUpdate();
	}
	else {
		MoveInit();
	}

	if (countFrame_ > iParameters_[IParameterNames::k2JumpExtensionFrame]) {
		jumpCount_ = 0;
	}

	velocity_.x = 0.0f;
	velocity_ += move;
	if (isAuto_) {
		velocity_.x = 0;
	}

	worldTransform_.translate_ += velocity_;

}

void Player::JumpInitialize() {
	if (kIs2Jump_) {
		if (countFrame_ > iParameters_[IParameterNames::k2JumpExtensionFrame]) {
			jumpCount_ = 0;
		}

		if (jumpCount_ >= 1) {
			jumpCount_++;
			velocity_ = {};
			velocity_.y += parameters_[FloatParameterNames::kJumpInitialVelocity] * parameters_[FloatParameterNames::k2JumpMagnification];
		}
		else {
			jumpCount_ = 1;
			velocity_ = {};
			velocity_.y += parameters_[FloatParameterNames::kJumpInitialVelocity];
		}
	}
	else {
		velocity_ = {};
		velocity_.y += parameters_[FloatParameterNames::kJumpInitialVelocity];
	}
	isReservationJump_ = false;
}

void Player::JumpUpdate() {

	if (isPrecedingEntry_) {
		if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
			isReservationJump_ = true;
		}
	}

	if (kIs2Jump_) {
		if (jumpCount_ >= 2) {
			if (isRight_) {
				velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX] * parameters_[FloatParameterNames::k2JumpMagnification];
				if (velocity_.x >= parameters_[FloatParameterNames::kJumpMaxSpeedX]) {
					velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX];
				}

				worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
			}
			else {
				velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX] * (-1) * parameters_[FloatParameterNames::k2JumpMagnification];
				if (velocity_.x <= parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1)) {
					velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1);
				}
				worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
			}
		}
		else {
			if (isRight_) {
				velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX];
				if (velocity_.x >= parameters_[FloatParameterNames::kJumpMaxSpeedX]) {
					velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX];
				}

				worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
			}
			else {
				velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX] * (-1);
				if (velocity_.x <= parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1)) {
					velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1);
				}
				worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
			}
		}
	}
	else {
		if (isRight_) {
			velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX];
			if (velocity_.x >= parameters_[FloatParameterNames::kJumpMaxSpeedX]) {
				velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX];
			}

			worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
		}
		else {
			velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX] * (-1);
			if (velocity_.x <= parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1)) {
				velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1);
			}
			worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
		}
	}

	if (velocity_.y <= 0) {
		velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
		if (velocity_.y <= -1.5f) {
			velocity_.y = -1.5f;
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kGravity];
	}

	worldTransform_.translate_ += velocity_;
}

void Player::GripWallInitialize() {
	worldTransform_.rotation_ = {};
	if (isRight_) {
		worldTransform_.rotation_.y = 0.0f;
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	}

	velocity_ = {};
	countFrame_ = 0;

	if (preState_ == State::kNormal || preState_ == State::kJump) {
		jumpCount_ = 0;
	}

	isDown_ = false;
}

void Player::GripWallUpdate() {

	countFrame_++;

	if (countFrame_ > iParameters_[IParameterNames::k2JumpExtensionFrame]) {
		jumpCount_ = 0;
	}

	Input* input = Input::GetInstance();

	Vector2 move = input->GetGamePadLStick();

	if (isRight_) {
		// 右の壁

	}
	else {
		// 左の壁

	}

	if (kIsWallDown_) {

		if (input->PressingGamePadButton(Input::GamePadButton::A)) {
			auto playHandle = Audio::GetInstance()->SoundPlayWave(jumpSoundHandle_);
			Audio::GetInstance()->SetValume(playHandle, 1.0f);
			if (jumpCount_ >= 1) {
				Audio::GetInstance()->SetPitch(playHandle, 1.5f);
			}
			else {
				Audio::GetInstance()->SetPitch(playHandle, 1.0f);
			}
			velocity_.y += parameters_[FloatParameterNames::kWallGravity];
		}
		else if (input->ReleasedGamePadButton(Input::GamePadButton::A)) {
			auto playHandle = Audio::GetInstance()->SoundPlayWave(jumpSoundHandle_);
			Audio::GetInstance()->SetValume(playHandle, 1.0f);
			if (jumpCount_ >= 1) {
				Audio::GetInstance()->SetPitch(playHandle, 1.5f);
			}
			else {
				Audio::GetInstance()->SetPitch(playHandle, 1.0f);
			}
			if (isRight_) {
				// 右の壁
				if (move.x <= -0.3f) {
					// 左上
					isRight_ = false;
					StateRequest(State::kWallSideJump);
				}
				else {
					// 上
					StateRequest(State::kWallJump);
				}

			}
			else {
				// 左の壁

				if (move.x >= 0.3f) {
					// 右上
					isRight_ = true;
					StateRequest(State::kWallSideJump);
				}
				else {
					// 上
					StateRequest(State::kWallJump);
				}
			}
		}

	}
	else {
		if (countFrame_ >= iParameters_[IParameterNames::kGripStayTime] || isDown_ || move.y <= -0.3f) {
			velocity_.y += parameters_[FloatParameterNames::kWallGravity];
			isDown_ = true;
		}

		if ((input->PressedGamePadButton(Input::GamePadButton::A) || isAuto_ || isReservationJump_)) {
			auto playHandle = Audio::GetInstance()->SoundPlayWave(jumpSoundHandle_);
			if (jumpCount_ >= 1) {
				Audio::GetInstance()->SetPitch(playHandle, 1.5f);
			}
			else {
				Audio::GetInstance()->SetPitch(playHandle, 1.0f);
			}
			if (isRight_) {
				// 右の壁
				if (move.x <= -0.3f) {
					// 左上
					isRight_ = false;
					StateRequest(State::kWallSideJump);
				}
				else {
					// 上
					StateRequest(State::kWallJump);
				}

			}
			else {
				// 左の壁

				if (move.x >= 0.3f) {
					// 右上
					isRight_ = true;
					StateRequest(State::kWallSideJump);
				}
				else {
					// 上
					StateRequest(State::kWallJump);
				}
			}

			//if (isRight_) {
			//	// 右の壁

			//	if (move.y >= 0.0f) {
			//		if (move.x <= -0.3f) {
			//			// 左上
			//			isRight_ = false;
			//			StatusRequest(Status::kWallSideJump);
			//		}
			//		else {
			//			// 上
			//			StatusRequest(Status::kWallJump);
			//		}
			//	}
			//	else if (move.y <= -0.4f){
			//		// 左下
			//		isRight_ = false;
			//		StatusRequest(Status::kWallDownJump);
			//	}
			//	else {
			//		StatusRequest(Status::kWallJump);
			//	}

			//}
			//else {
			//	// 左の壁

			//	if (move.y >= 0.0f) {
			//		if (move.x >= 0.3f) {
			//			// 右上
			//			isRight_ = true;
			//			StatusRequest(Status::kWallSideJump);
			//		}
			//		else {
			//			// 上
			//			StatusRequest(Status::kWallJump);
			//		}
			//	}
			//	else if (move.y <= -0.4f) {
			//		// 右下
			//		isRight_ = true;
			//		StatusRequest(Status::kWallDownJump);
			//	}
			//	else {
			//		StatusRequest(Status::kWallJump);
			//	}
			//}
		}
	}

	worldTransform_.translate_ += velocity_;
}

void Player::WallJumpInitialize() {
	velocity_ = {};

	//if (isRight_) {
	//	// 右の壁
	//	velocity_.x -= parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
	//}
	//else {
	//	// 左の壁
	//	velocity_.x += parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
	//}
	isReservationJump_ = false;

	if (kIs2WallJump_) {
		if (countFrame_ > iParameters_[IParameterNames::k2JumpExtensionFrame]) {
			jumpCount_ = 0;
		}

		if (jumpCount_ >= 1) {
			jumpCount_++;
			if (isRight_) {
				// 右の壁
				velocity_.x -= parameters_[FloatParameterNames::kWallJumpInitialVelocityX] * parameters_[FloatParameterNames::k2WallJumpMagnification];
			}
			else {
				// 左の壁
				velocity_.x += parameters_[FloatParameterNames::kWallJumpInitialVelocityX] * parameters_[FloatParameterNames::k2WallJumpMagnification];
			}
			velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY] * parameters_[FloatParameterNames::k2WallJumpMagnification];
		}
		else {
			jumpCount_ = 1;
			if (isRight_) {
				// 右の壁
				velocity_.x -= parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
			}
			else {
				// 左の壁
				velocity_.x += parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
			}
			velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY];
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY];
	}

}

void Player::WallJumpUpdate() {

	if (isPrecedingEntry_) {
		if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
			isReservationJump_ = true;
		}
	}

	if (isRight_) {
		// 右の壁
		if (velocity_.x >= 0) {
			velocity_.x -= parameters_[FloatParameterNames::kFallingGravity];
			if (velocity_.x > parameters_[FloatParameterNames::kJumpMaxSpeedX]) {
				velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX];
			}
		}
		else {
			velocity_.x -= parameters_[FloatParameterNames::kGravity];
		}
		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	else {
		// 左の壁
		if (velocity_.x <= 0) {
			velocity_.x += parameters_[FloatParameterNames::kFallingGravity];
			if (velocity_.x < parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1)) {
				velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1);
			}
		}
		else {
			velocity_.x += parameters_[FloatParameterNames::kGravity];
		}

		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	if (velocity_.y <= 0) {
		velocity_.y += parameters_[FloatParameterNames::kGravity];
		if (velocity_.y < -1.5f) {
			velocity_.y = -1.5f;
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
	}

	worldTransform_.translate_ += velocity_;
}

void Player::WallSideJumpInitialize() {
	velocity_ = {};
	velocity_.y += parameters_[FloatParameterNames::kJumpInitialVelocity];
	jumpCount_ = 0;
	isReservationJump_ = false;
}

void Player::WallSideJumpUpdate() {

	if (isPrecedingEntry_) {
		if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
			isReservationJump_ = true;
		}
	}

	if (isRight_) {
		velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX];
		if (velocity_.x >= parameters_[FloatParameterNames::kJumpMaxSpeedX]) {
			velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX];
		}
		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	else {
		velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX] * (-1);
		if (velocity_.x <= parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1)) {
			velocity_.x = parameters_[FloatParameterNames::kJumpMaxSpeedX] * (-1);
		}
		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
	}

	velocity_.y += parameters_[FloatParameterNames::kGravity];
	if (velocity_.y <= -1.5f) {
		velocity_.y = -1.5f;
	}

	/*if (velocity_.y <= 0) {
		velocity_.y += parameters_[FloatParameterNames::kGravity];
		if (velocity_.y <= -1.5f) {
			velocity_.y = -1.5f;
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
	}*/

	worldTransform_.translate_ += velocity_;
}

void Player::WallDownJumpInitialize() {
	velocity_ = {};
	isReservationJump_ = false;

	if (isRight_) {
		// 右の壁
		velocity_.x += parameters_[FloatParameterNames::kJumpInitialVelocity];
	}
	else {
		// 左の壁
		velocity_.x -= parameters_[FloatParameterNames::kJumpInitialVelocity];
	}
}

void Player::WallDownJumpUpdate() {

	if (isPrecedingEntry_) {
		if (Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::A)) {
			isReservationJump_ = true;
		}
	}

	if (isRight_) {
		velocity_.x -= parameters_[FloatParameterNames::kJumpAccelerationX];
		if (velocity_.x <= 0.2f) {
			velocity_.x = 0.2f;
		}
		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	else {
		velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX];
		if (velocity_.x >= -0.2f) {
			velocity_.x = -0.2f;
		}
		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
	}

	if (velocity_.y <= 0) {
		velocity_.y += parameters_[FloatParameterNames::kGravity];
		if (velocity_.y <= -1.5f) {
			velocity_.y = -1.5f;
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
	}

	worldTransform_.translate_ += velocity_;
}

void Player::ParticleInitialize() {

	emitter_ = new Emitter();
	emitter_->aliveTime = 1;
	emitter_->spawn.position = worldTransform_.worldPos_;
	emitter_->spawn.rangeX = 0.0f;
	emitter_->spawn.rangeY = 0.0f;
	emitter_->scale.startScale = { 1.0f,1.0f,1.0f };
	emitter_->scale.endScale = { 0.01f,0.01f,0.01f };
	emitter_->inOnce = 2;
	//emitter_->angle.start = DegToRad(0.0f);
	//emitter_->angle.end = DegToRad(180.0f);
	emitter_->isAlive = true;
	particleMotion_ = new ParticleMotion();
	particleMotion_->color.startColor = { rnd_.NextFloatRange(0.0f,0.02f),rnd_.NextFloatRange(0.0f,0.02f),rnd_.NextFloatRange(0.0f,0.01f),rnd_.NextFloatRange(0.0f,0.5f) };
	particleMotion_->color.endColor = { rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.0f,0.0f),rnd_.NextFloatRange(0.0f,0.1f),rnd_.NextFloatRange(0.0f,0.0f) };
	particleMotion_->color.currentColor = particleMotion_->color.startColor;
	particleMotion_->rotate.addRotate = { 0.0f,0.0f,0.2f };
	particleMotion_->rotate.currentRotate = { 0.0f,0.0f,0.0f };
	particleMotion_->acceleration_ = { 0.0f,0.0f,0.0f };
	//particleMotion_->velocity.speed = 1.0f;
	//particleMotion_->velocity.randomRange = 0.0f;
	particleMotion_->aliveTime.time = 20;
	particleMotion_->aliveTime.randomRange = 5;
	particleMotion_->isAlive = true;
	ParticleManager::GetInstance()->AddParticle(emitter_, particleMotion_, particleTextureHandle_);

	isCreateParticle_ = true;
}

void Player::ParticleUpdate() {
	if (!isDead_ &&
		state_ != State::kDeadMove) {
		emitter_->aliveTime = 1;
		emitter_->spawn.position = worldTransform_.worldPos_;
		if (jumpCount_ >= 2) {
			particleMotion_->color.startColor = { rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.3f,0.6f) };
			emitter_->scale.startScale = { 1.5,1.5f,1.5f };
		}
		else {
			particleMotion_->color.startColor = { rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.8f,1.0f),rnd_.NextFloatRange(0.3f,0.6f) };
			emitter_->scale.startScale = { 0.8f,0.8f,0.8f };
		}
		//particleMotion_->color.endColor = { rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.8f,1.0f),rnd_.NextFloatRange(0.0f,0.4f) };
		particleMotion_->color.currentColor = particleMotion_->color.startColor;
	}
}

void Player::WallParticleCreate(const Vector2& vector) {
	if (isCreateParticle_) {
		isCreateParticle_ = false;

		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();

		emitter->aliveTime = 1;
		emitter->spawn.position = worldTransform_.worldPos_;
		emitter->spawn.rangeX = 1.5f;
		emitter->spawn.rangeY = 1.5f;
		emitter->scale.startScale = { 0.3f,0.3f,0.3f };
		emitter->scale.endScale = { 0.01f,0.01f,0.01f };
		emitter->inOnce = 20;
		// 右の壁
		if (vector.x < 0) {
			emitter->angle.start = DegToRad(90.0f);
			emitter->angle.end = DegToRad(270.0f);
			particleMotion->acceleration_ = { 0.0f,-0.08f,0.0f };
			particleMotion->velocity.speed = 0.8f;
			particleMotion->velocity.randomRange = 0.3f;
		}
		// 左の壁
		else if (vector.x > 0) {
			emitter->angle.start = -DegToRad(90.0f);
			emitter->angle.end = DegToRad(90.0f);
			particleMotion->acceleration_ = { 0.0f,-0.08f,0.0f };
			particleMotion->velocity.speed = 0.8f;
			particleMotion->velocity.randomRange = 0.3f;
		}
		// 天井
		else if (vector.y > 0) {
			emitter->angle.start = DegToRad(0.0f);
			emitter->angle.end = DegToRad(180.0f);
			particleMotion->acceleration_ = { 0.0f,-0.05f,0.0f };
			particleMotion->velocity.speed = 0.5f;
			particleMotion->velocity.randomRange = 0.2f;
		}
		// 床
		else if (vector.y < 0) {
			emitter->angle.start = -DegToRad(210.0f);
			emitter->angle.end = DegToRad(30.0f);
			particleMotion->acceleration_ = { 0.0f,-0.05f,0.0f };
			particleMotion->velocity.speed = 0.2f;
			particleMotion->velocity.randomRange = 0.05f;
		}
		emitter->isAlive = true;

		particleMotion->color.startColor = { rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.0f,0.2f),rnd_.NextFloatRange(0.5f,0.8f),1.0f };
		particleMotion->color.endColor = { rnd_.NextFloatRange(0.0f,0.05f),rnd_.NextFloatRange(0.0f,0.05f),rnd_.NextFloatRange(0.1f,0.5f),0.0f };
		particleMotion->color.currentColor = particleMotion->color.startColor;
		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.2f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

		particleMotion->aliveTime.time = 60;
		particleMotion->aliveTime.randomRange = 55;
		particleMotion->isAlive = true;
		ParticleManager::GetInstance()->AddParticle(emitter, particleMotion, particleTextureHandle_);
	}
}
void Player::DeathParticleCreate() {
	Emitter* emitter = new Emitter();
	ParticleMotion* particleMotion = new ParticleMotion();

	emitter->aliveTime = 1;
	emitter->flameInterval = 0;
	emitter->spawn.position = worldTransform_.worldPos_;
	emitter->spawn.rangeX = 0.0f;
	emitter->spawn.rangeY = 0.0f;
	emitter->scale.startScale= { 0.5f,0.5f,0.5f };
	emitter->scale.endScale= { 0.01f,0.01f,0.01f };
	emitter->inOnce = 60;
	emitter->angle.start = DegToRad(0.0f);
	emitter->angle.end = DegToRad(360.0f);
	emitter->isAlive = true;

	particleMotion->color.startColor = { rnd_.NextFloatRange(0.0f,0.0f),rnd_.NextFloatRange(0.0f,1.0f),rnd_.NextFloatRange(0.0f,1.0f),1.0f };
	particleMotion->color.endColor = { rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.0f,0.5f),rnd_.NextFloatRange(0.0f,0.5f),0.0f };
	particleMotion->color.currentColor = particleMotion->color.startColor;
	particleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
	particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

	particleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
	particleMotion->velocity.speed = 0.2f;
	particleMotion->velocity.randomRange = 0.0f;

	particleMotion->aliveTime.time = 30;
	particleMotion->aliveTime.randomRange = 0;
	particleMotion->isAlive = true;
	ParticleManager::GetInstance()->AddParticle(emitter, particleMotion, 0);

	Emitter* horizontalEmitter = new Emitter();
	ParticleMotion* horizontalParticleMotion = new ParticleMotion();

	horizontalEmitter->aliveTime = 1;
	horizontalEmitter->flameInterval = 0;
	horizontalEmitter->spawn.position = worldTransform_.worldPos_;
	horizontalEmitter->spawn.rangeX = 0.0f;
	horizontalEmitter->spawn.rangeY = 0.0f;
	horizontalEmitter->scale.startScale = { 0.0f,0.0f,0.0f };
	horizontalEmitter->scale.interimScale = { 2.5f,500.0f,10.0f };
	horizontalEmitter->scale.endScale = { 0.0f,0.0f,0.0f };
	horizontalEmitter->inOnce = 1;
	horizontalEmitter->angle.start = 0.0f;
	horizontalEmitter->angle.end = 0.0f;
	horizontalEmitter->isAlive = true;

	horizontalParticleMotion->color.startColor = { 0.2f,0.2f,0.2f,1.0f };
	horizontalParticleMotion->color.endColor = { 0.2f,0.2f,0.2f,1.0f };
	horizontalParticleMotion->color.currentColor = horizontalParticleMotion->color.startColor;
	horizontalParticleMotion->scale.startScale = { 0.0f,0.0f,0.0f };
	horizontalParticleMotion->scale.interimScale = { 2.5f,500.0f,10.0f };
	horizontalParticleMotion->scale.endScale = { 0.0f,0.0f,0.0f };
	horizontalParticleMotion->scale.currentScale = horizontalParticleMotion->scale.startScale;
	horizontalParticleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
	horizontalParticleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

	horizontalParticleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
	horizontalParticleMotion->velocity.speed = 0.0f;
	horizontalParticleMotion->velocity.randomRange = 0.0f;
	horizontalParticleMotion->acceleration_ = {};
	horizontalParticleMotion->aliveTime.time = 30;
	horizontalParticleMotion->aliveTime.randomRange = 0;
	horizontalParticleMotion->isAlive = true;
	ParticleManager::GetInstance()->AddParticle(horizontalEmitter, horizontalParticleMotion, 0);

	Emitter* verticalEmitter = new Emitter();
	ParticleMotion* verticalParticleMotion = new ParticleMotion();

	verticalEmitter->aliveTime = 1;
	verticalEmitter->flameInterval = 0;
	verticalEmitter->spawn.position = worldTransform_.worldPos_;
	verticalEmitter->spawn.rangeX = 0.0f;
	verticalEmitter->spawn.rangeY = 0.0f;
	verticalEmitter->scale.startScale = { 0.0f,0.0f,0.0f };
	verticalEmitter->scale.interimScale= { 500.0f,2.5f,10.0f };
	verticalEmitter->scale.endScale= { 0.0f,0.0f,0.0f };
	verticalEmitter->inOnce = 1;
	verticalEmitter->angle.start = 0.0f;
	verticalEmitter->angle.end = 0.0f;
	verticalEmitter->isAlive = true;

	verticalParticleMotion->color.startColor = { 0.2f,0.2f,0.2f,1.0f };
	verticalParticleMotion->color.endColor = { 0.2f,0.2f,0.2f,1.0f };
	verticalParticleMotion->color.currentColor = verticalParticleMotion->color.startColor;
	verticalParticleMotion->rotate.addRotate = { 0.0f,0.0f,0.0f };
	verticalParticleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };

	verticalParticleMotion->acceleration_ = { 0.0f,0.0f,0.0f };
	verticalParticleMotion->velocity.speed = 0.0f;
	verticalParticleMotion->velocity.randomRange = 0.0f;
	verticalParticleMotion->acceleration_ = {};
	verticalParticleMotion->aliveTime.time = 30;
	verticalParticleMotion->aliveTime.randomRange = 0;
	verticalParticleMotion->isAlive = true;
	ParticleManager::GetInstance()->AddParticle(verticalEmitter, verticalParticleMotion, 0);

}
void Player::SoundInitialize() {
	auto audio = Audio::GetInstance();
	deathSoundHandle_ = audio->SoundLoadWave("SE/death.wav");
	jumpSoundHandle_ = audio->SoundLoadWave("SE/jump.wav");
	clearSoundHandle_ = audio->SoundLoadWave("SE/clear.wav");
	isChangeCamera_ = true;
}

void Player::ClearMoveInitialize() {
	countFrame_ = 0;
	preClearPos_ = { worldTransform_.translate_.x,worldTransform_.translate_.y };
	clearRot_ = 0.0f;
	preClearScale_ = worldTransform_.scale_;
	isChangeCamera_ = true;
	isCollisionGoal_ = true;
	Audio::GetInstance()->SoundPlayWave(clearSoundHandle_);
}

void Player::ClearMoveUpdate() {
	countFrame_++;

	Vector2 pos = Ease::UseEase(preClearPos_, goalPos_, countFrame_, iParameters_[IParameterNames::kClearFrame], Ease::EaseType::Constant);

	if (isRight_) {
		clearRot_ -= parameters_[FloatParameterNames::kClearRotateSpeed];
		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	else {
		clearRot_ += parameters_[FloatParameterNames::kClearRotateSpeed];
		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
	}

	worldTransform_.scale_ = Ease::UseEase(preClearScale_, Vector3{}, countFrame_, iParameters_[IParameterNames::kClearFrame], Ease::EaseType::Constant);

	pos -= goalPos_;

	worldTransform_.translate_.x = pos.x * std::cosf(clearRot_) - pos.y * std::sinf(clearRot_) + goalPos_.x;
	worldTransform_.translate_.y = pos.x * std::sinf(clearRot_) + pos.y * std::cosf(clearRot_) + goalPos_.y;

	if (countFrame_ >= iParameters_[IParameterNames::kClearFrame]) {
		//Initialize();
		isClear_ = true;
	}
}

void Player::DeadModeInitialize() {
	deathAnimationTime_ = 0.0f;
	MaxDeathAnimationTime = 60.0f;
	DeathParticleCreate();
	worldTransform_.scale_ = {};
	uint32_t playHandle = Audio::GetInstance()->SoundPlayWave(deathSoundHandle_);
	Audio::GetInstance()->SetValume(playHandle,2.0f);
}

void Player::DeadModeUpdate() {
	if (deathAnimationTime_ >= MaxDeathAnimationTime) {
		//Initialize();
		isDead_ = true;
		ParticleInitialize();
	}
	deathAnimationTime_ += 1.0f;
	// プレイヤーが死んだときの処理

#ifdef _DEBUG

#endif // _DEBUG

}

void Player::FloarAndWallInit() {

	noTatchCountFrame_ = 0;

	worldTransform_.rotation_ = {};
	if (isRight_) {
		worldTransform_.rotation_.y = 0.0f;
	}
	else {
		worldTransform_.rotation_.y = std::numbers::pi_v<float>;
	}
	velocity_ = {};
	countFrame_ = 0;

	jumpCount_ = 0;
}

void Player::FloarAndWallUpdate() {

	Input* input = Input::GetInstance();

	Vector2 move = input->GetGamePadLStick();

	if ((input->PressedGamePadButton(Input::GamePadButton::A) || isAuto_ || isReservationJump_)) {
		auto playHandle = Audio::GetInstance()->SoundPlayWave(jumpSoundHandle_);
		Audio::GetInstance()->SetValume(playHandle, 1.0f);
		if (jumpCount_ >= 1) {
			Audio::GetInstance()->SetPitch(playHandle, 1.5f);
		}
		else {
			Audio::GetInstance()->SetPitch(playHandle, 1.0f);
		}

		if ((isRight_ && move.x >= 0) || (!isRight_ && move.x <= 0)) {
			StateRequest(State::kWallJump);
		}
		else {
			StateRequest(State::kJump);

			if (move.x > 0) {
				isRight_ = true;
				worldTransform_.rotation_.y = 0.0f;
			}
			else if (move.x < 0) {
				isRight_ = false;
				worldTransform_.rotation_.y = std::numbers::pi_v<float>;
			}
		}
	}
	else {
		if (!isRight_ && move.x > 0.2f) {
			StateRequest(State::kNormal);
			isRight_ = true;
			worldTransform_.rotation_.y = 0.0f;
		}
		else if (isRight_ && move.x < -0.2f) {
			StateRequest(State::kNormal);
			isRight_ = false;
			worldTransform_.rotation_.y = std::numbers::pi_v<float>;
		}
	}


}

void (Player::* Player::spStateInitFuncTable[])() {
	&Player::NormalInitialize,
		& Player::JumpInitialize,
		& Player::GripWallInitialize,
		& Player::WallJumpInitialize,
		& Player::WallSideJumpInitialize,
		& Player::WallDownJumpInitialize,
		& Player::ClearMoveInitialize,
		& Player::DeadModeInitialize,
		& Player::FloarAndWallInit,
};

void (Player::* Player::spStateUpdateFuncTable[])() {
	&Player::NormalUpdate,
		& Player::JumpUpdate,
		& Player::GripWallUpdate,
		& Player::WallJumpUpdate,
		& Player::WallSideJumpUpdate,
		& Player::WallDownJumpUpdate,
		& Player::ClearMoveUpdate,
		& Player::DeadModeUpdate,
		& Player::FloarAndWallUpdate,
};

void Player::Update() {
	ApplyGlobalVariable();

	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		preState_ = state_;
		stateRequest_ = std::nullopt;

		isCreateParticle_ = true;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();

#ifdef _DEBUG
	if (Input::GetInstance()->PressedKey(DIK_R) || Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::Y)) {
		Initialize();
	}
#endif // _DEBUG
	UpdateMatrix();

	SetCollider();

	ParticleUpdate();

	// クリアタイム
	clearTime_++;
}

void Player::Draw(const ViewProjection& viewProjection) {
	if (!isDead_) {
		for (int i = 0; i < Parts::kCountParts; i++) {
			auto material = models_[i]->GetMaterial()->GetMaterial();
			if (jumpCount_ >= 2) {
				material->color_ = { 255.0f,69.0f,0.0f,1.0f };

			} else {
				material->color_ = { 1.0f ,1.0f ,1.0f ,1.0f };
			}
			models_[i]->Draw(modelWorldTransforms_[i], viewProjection);
			models_[i]->GetMaterial()->SetMaterial(*material);
		}
	}
}

void Player::DrawUI(const ViewProjection& viewProjection) {}

void Player::StateRequest(State state) {
	if (state_ != state) {
		stateRequest_ = state;
	}
}


