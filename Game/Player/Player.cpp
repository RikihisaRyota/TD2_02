#include "Player.h"

#include "Input.h"
#include "GlobalVariables/GlobalVariables.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "Collision/CollisionConfig.h"
#include "ModelManager.h"
#include "TextureManager.h"
// 俺が追加した
#include "ParticleManager.h"
#include "MyMath.h"

Player::Player() {

	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::RIGID_BODY);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionAttributeBlock);

	models_.emplace_back((ModelManager::GetInstance()->GetModel("player")));
	face_.reset(PlaneRenderer::Create());
	faceTextureHandle_[0] = TextureManager::Load("Resources/Textures/playerFaceRight.png");
	faceTextureHandle_[1] = TextureManager::Load("Resources/Textures/playerFaceLeft.png");

	faceWorldTransform_.Initialize();
	faceWorldTransform_.scale_ = { 2.0f,2.0f,2.0f };
	faceWorldTransform_.translate_.z = -1.5f;
	faceWorldTransform_.UpdateMatrix();
	isPlayerFaceRight_ = false;

	modelWorldTransforms_.push_back(WorldTransform());
	modelWorldTransforms_[Parts::kMain].Initialize();

	worldTransform_.Initialize();
	worldTransform_.translate_ = { 50.0f,20.0f,0.0f };
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	modelWorldTransforms_[Parts::kMain].parent_ = &worldTransform_;
	faceWorldTransform_.parent_ = &worldTransform_;
	UpdateMatrix();

	isJump_ = true;
	isRight_ = true;
	velocity_ = {};
	parameters_[FloatParameterNames::kMoveSpeed] = 0.1f;
	parameters_[FloatParameterNames::kJumpInitialVelocity] = 1.0f;
	parameters_[FloatParameterNames::kGravity] = -0.02f;
	parameters_[FloatParameterNames::kFallingGravity] = -0.01f;

	v3Parameters_[V3ParameterNames::kInitialPos] = { 50.0f,20.0f,0.0f };
	preInitialPos_ = v3Parameters_[V3ParameterNames::kInitialPos];

	SetGlobalVariable();
}

void Player::Initialize()
{
	StateRequest(State::kNormal);

	worldTransform_.translate_ = v3Parameters_[kInitialPos];
	worldTransform_.scale_ = { 1.0f,1.0f,1.0f };
	worldTransform_.rotation_ = {};

	UpdateMatrix();

	jumpCount_ = 0;
	isJump_ = true;
	velocity_ = {};
}

void Player::UpdateMatrix() {
	worldTransform_.UpdateMatrix();

	for (int i = 0; i < Parts::kCountParts; i++) {
		modelWorldTransforms_[i].UpdateMatrix();
	}
	faceWorldTransform_.UpdateMatrix();
}

void Player::OnCollision()
{

	if (editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y == 0) {
		StateRequest(State::kNormal);
	}
	else if (velocity_.x != 0 && editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x == 0) {
		StateRequest(State::kGripWall);
		if (velocity_.x > 0) {
			isRight_ = true;
		}
		else if (velocity_.x < 0) {
			isRight_ = false;
		}
	}
	else {
		StateRequest(State::kNormal);
	}
	worldTransform_.translate_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].x;
	worldTransform_.translate_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS].y;

	velocity_.x = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].x;
	velocity_.y = editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY].y;

	UpdateMatrix();

	if ((editInfo_.collisionMask_ & kCollisionAttributeBlock) >= 0b1) {

		for (uint32_t no : editInfo_.i32Info_) {

			if (no == uint32_t(MapChip::Blocks::kRedBlock)) {

				StateRequest(State::kClearMove);
				break;
			}


		}
	}

}

void Player::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y }, 
		Vector2{ worldTransform_.scale_.x,worldTransform_.scale_.y },Vector2{ velocity_.x,velocity_.y });

	CollisionManager::GetInstance()->SetCollider(this);
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

void Player::NormalInitialize() {
	worldTransform_.rotation_ = {};
	countFrame_ = 0;
	if (preState_ != State::kJump) {
		jumpCount_ = 0;
	}
}

void Player::NormalUpdate()
{
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
		isPlayerFaceRight_ = false;
	}
	else if (move.x < 0) {
		isPlayerFaceRight_ = true;
	}

	move.x *= parameters_[FloatParameterNames::kMoveSpeed];

	if (input->PressedGamePadButton(Input::GamePadButton::A) && !isJump_) {

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

	if (move.x > 0) {
		isRight_ = true;
	}
	else if (move.x < 0) {
		isRight_ = false;
	}

	velocity_.x = 0.0f;
	velocity_ += move;

	worldTransform_.translate_ += velocity_;

}

void Player::JumpInitialize()
{
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

}

void Player::JumpUpdate()
{
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
	velocity_ = {};
	countFrame_ = 0;

	if (preState_ == State::kNormal || preState_ == State::kJump) {
		jumpCount_ = 0;
	}
}

void Player::GripWallUpdate() {

	countFrame_++;

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
			velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
		}
		else if (input->ReleasedGamePadButton(Input::GamePadButton::A)) {
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
		if (countFrame_ >= iParameters_[IParameterNames::kGripStayTime]) {
			velocity_.y += parameters_[FloatParameterNames::kFallingGravity];
		}

		if (input->PressedGamePadButton(Input::GamePadButton::A)) {

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

	if (isRight_) {
		// 右の壁
		velocity_.x -= parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
	}
	else {
		// 左の壁
		velocity_.x += parameters_[FloatParameterNames::kWallJumpInitialVelocityX];
	}

	if (kIs2WallJump_) {
		if (countFrame_ > iParameters_[IParameterNames::k2JumpExtensionFrame]) {
			jumpCount_ = 0;
		}

		if (jumpCount_ >= 1) {
			jumpCount_++;
			velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY] * parameters_[FloatParameterNames::k2JumpMagnification];
		}
		else {
			jumpCount_ = 1;
			velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY];
		}
	}
	else {
		velocity_.y += parameters_[FloatParameterNames::kWallJumpInitialVelocityY];
	}
	
}

void Player::WallJumpUpdate()
{

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
		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
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

		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
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
}

void Player::WallSideJumpUpdate() {
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
	if (isRight_) {
		velocity_.x -= parameters_[FloatParameterNames::kJumpAccelerationX];
		if (velocity_.x <= 0.2f) {
			velocity_.x = 0.2f;
		}
		worldTransform_.rotation_.z += parameters_[FloatParameterNames::kJumpRotateSpeed];
	}
	else {
		velocity_.x += parameters_[FloatParameterNames::kJumpAccelerationX];
		if (velocity_.x >= -0.2f) {
			velocity_.x = -0.2f;
		}
		worldTransform_.rotation_.z -= parameters_[FloatParameterNames::kJumpRotateSpeed];
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

void Player::MoveParticle() {
	if (flameCount_ <= 0) {
		Emitter* emitter = new Emitter();
		emitter->aliveTime = 1;
		emitter->spawn.position = worldTransform_.worldPos_;
		emitter->spawn.rangeX = 0.5f;
		emitter->spawn.rangeY = 0.5f;
		emitter->inOnce = 4;
		emitter->isAlive = true;
		ParticleMotion* particleMotion = new ParticleMotion();
		//particleMotion->angle.start = DegToRad(0.0f);
		//particleMotion->angle.end = DegToRad(180.0f);
		particleMotion->color.startColor = { 0.0f,1.0f,0.8f,1.0f };
		particleMotion->color.endColor = { 0.0f,1.0f,0.8f,0.0f };
		particleMotion->color.currentColor = particleMotion->color.startColor;
		particleMotion->scale.startScale = { 0.5f,0.5f,0.5f };
		particleMotion->scale.endScale = { 0.01f,0.01f,0.01f };
		particleMotion->scale.currentScale = particleMotion->scale.startScale;
		particleMotion->rotate.addRotate = { 0.0f,0.0f,0.2f };
		particleMotion->rotate.currentRotate = { 0.0f,0.0f,0.0f };
		//particleMotion->velocity.speed = 1.0f;
		//particleMotion->velocity.randomRange = 0.0f;
		particleMotion->aliveTime.time = 20;
		particleMotion->aliveTime.randomRange = 5;
		particleMotion->isAlive = true;
		ParticleManager::GetInstance()->AddParticle(emitter, particleMotion, 0);
		flameCount_ = kMaxFlameTime;
	}
	else {
		flameCount_--;
	}
}

void Player::Update() {


void Player::ClearMoveInitialize()
{
	countFrame_ = 0;
}
void Player::ClearMoveUpdate()
{
	countFrame_++;
	if (countFrame_ == 60) {
		Initialize();
	}
}

void (Player::* Player::spStateInitFuncTable[])() {
	&Player::NormalInitialize,
	&Player::JumpInitialize,
	&Player::GripWallInitialize,
	&Player::WallJumpInitialize,
	&Player::WallSideJumpInitialize,
	&Player::WallDownJumpInitialize,
	&Player::ClearMoveInitialize,
};

void (Player::* Player::spStateUpdateFuncTable[])() {
	&Player::NormalUpdate,
	&Player::JumpUpdate,
	&Player::GripWallUpdate,
	&Player::WallJumpUpdate,
	&Player::WallSideJumpUpdate,
	&Player::WallDownJumpUpdate,
	&Player::ClearMoveUpdate,
};

void Player::Update()
{

	ApplyGlobalVariable();

	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		preState_ = state_;
		stateRequest_ = std::nullopt;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();

#ifdef _DEBUG
	if (Input::GetInstance()->PressedKey(DIK_R) || Input::GetInstance()->PressedGamePadButton(Input::GamePadButton::Y)) {
		Initialize();
	}
#endif // _DEBUG
	UpdateMatrix();

	SetCollider();

	MoveParticle();
}

void Player::Draw(const ViewProjection& viewProjection) {
	for (int i = 0; i < Parts::kCountParts; i++) {
		models_[i]->Draw(modelWorldTransforms_[i], viewProjection);
	}
}

void Player::DrawUI(const ViewProjection& viewProjection) {
	face_->Draw(faceWorldTransform_,viewProjection,faceTextureHandle_[isPlayerFaceRight_]);
}

void Player::StateRequest(State state)
{
	if (state_ != state) {
		stateRequest_ = state;
	}
}


