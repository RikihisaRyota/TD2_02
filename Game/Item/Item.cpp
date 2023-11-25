#include "Item.h"
#include "Collision/CollisionConfig.h"
#include "Collision/CollisionManager.h"
#include "Collision/ColliderShapes/ColliderShapeBox2D.h"
#include "GlobalVariables/GlobalVariables.h"
#include "ModelManager.h"
#include "Ease/Ease.h"
#include "MapChip.h"
#include <numbers>
#include "TextureManager.h"

Item::Item()
{
	shapeType_ = std::make_unique<ColliderShapeBox2D>(BaseColliderShapeType::ColliderType::COLLIDER);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	for (int i = 0; i < EditInfo::EditEnumV2::V2COUNT; i++) {
		editInfo_.v2Paras_.push_back(Vector2());
	}

	SetCollisionAttribute(kCollisionAttributeItem);
	SetCollisionMask(kCollisionAttributePlayer);

	// main.cppのモデルマネージャーの番号
	model_.reset(ModelManager::GetInstance()->GetBlockModel(17));

	worldTransform_.Initialize();

	isLife_ = false;
	velocity_ = {};

	SetGlobalVariable();
}

Item::~Item()
{
	model_.release();
}

void Item::Init(const Vector3& pos)
{
	worldTransform_.Reset();
	worldTransform_.translate_ = pos;
	pos_ = pos;
	worldTransform_.UpdateMatrix();
	isLife_ = true;
	isDraw_ = true;
	stateRequest_ = State::kIsLife;
}

void Item::Update()
{
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG


	countFrame_++;

	if (stateRequest_) {
		state_ = stateRequest_.value();

		(this->*spStateInitFuncTable[static_cast<size_t>(state_)])();

		stateRequest_ = std::nullopt;
	}

	(this->*spStateUpdateFuncTable[static_cast<size_t>(state_)])();

	worldTransform_.UpdateMatrix();

	if (isLife_) {
		SetCollider();
	}
}

void Item::Draw(const ViewProjection& viewProjection)
{
	if (isDraw_) {
		model_->Draw(worldTransform_, viewProjection);
	}
}

void Item::OnCollision()
{

	if (isLife_) {
		isLife_ = false;
		stateRequest_ = State::kGet;
		ItemManager::GetInstance()->AddGetCount();
	}

}

void Item::SetCollider()
{
	shapeType_->SetV2Info(Vector2{ worldTransform_.translate_.x,worldTransform_.translate_.y },
		Vector2{ worldTransform_.scale_.x,worldTransform_.scale_.y }, Vector2{ 0.0f,0.0f });

	CollisionManager::GetInstance()->SetCollider(this);
}

void Item::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		globalVariables->AddItem(groupName_, fInfoNames_[i], fInfo_[i]);
	}

	ApplyGlobalVariable();
}

void Item::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}

	for (int i = 0; i < FInfoNames::kFInfoCount; i++) {
		fInfo_[i] = globalVariables->GetFloatValue(groupName_, fInfoNames_[i]);
	}
}

void Item::IsLifeInit()
{
	rotateTheta_ = 0.0f;
	translateTheta_ = 0.0f;
}

void Item::IsLifeUpdate()
{
	const float pi = std::numbers::pi_v<float>;

	float step = 2.0f * pi / iInfo_[IInfoNames::kTranslateFrame];

	translateTheta_ += step;

	translateTheta_ = std::fmod(translateTheta_, 2.0f * pi);

	const float amplitude = 0.2f;

	float translate = std::sinf(translateTheta_) * amplitude;

	worldTransform_.translate_.y = pos_.y + translate;

	worldTransform_.rotation_.y += fInfo_[FInfoNames::kRotateSpeed];

	worldTransform_.rotation_.y = std::fmod(worldTransform_.rotation_.y, 2.0f * pi);
}

void Item::GetInit()
{
}

void Item::GetUpdate()
{
	isDraw_ = false;
}



void (Item::* Item::spStateInitFuncTable[])() {
	&Item::IsLifeInit,
	&Item::GetInit,
};

void (Item::* Item::spStateUpdateFuncTable[])() {
	&Item::IsLifeUpdate,
	&Item::GetUpdate,
};

ItemManager* ItemManager::GetInstance()
{
	static ItemManager instance;
	return &instance;
}

void ItemManager::FirstInit()
{
	for (std::unique_ptr<Item>& item : items_) {
		item.reset();
		item = std::make_unique<Item>();
	}
	uint32_t tex = TextureManager::Load("Resources/Textures/retryUi.png");
	sprites_[SpriteNames::kItemSprite].reset(Sprite::Create(tex, Vector2{}, { 1.0f,1.0f,1.0f,1.0f }, { 0.5f,0.5f }));


	Init();
	SetGlobalVariable();
}

void ItemManager::Init()
{
	Clear();
	countFrame_ = 0;
}

void ItemManager::CreateItem(const Vector3& pos)
{
	for (std::unique_ptr<Item>& item : items_) {
		if (!item->IsLife()) {
			item->Init(pos);
			MaxItemCount_++;
			return;
		}
	}
}

void ItemManager::Clear()
{
	MaxItemCount_ = 0;
	getItemCount_ = 0;
	for (const std::unique_ptr<Item>& item : items_) {
		item->SetIsLife(false);
	}
}

void ItemManager::SetItem(const Vector3& pos)
{
	for (const std::unique_ptr<Item>& item : items_) {
		if (!item->IsLife()) {
			item->Init(pos);
			break;
		}
	}
}

void ItemManager::Update()
{
	countFrame_++;

	for (int i = 0; i < MaxItemCount_; i++) {
		items_[i]->Update();
	}

	/*for (const std::unique_ptr<Item>& item : items_) {
		item->Update();
	}*/
}

void ItemManager::Draw(const ViewProjection& viewProjection)
{
	/*for (const std::unique_ptr<Item>& item : items_) {
		item->Draw(viewProjection);
	}*/
	for (int i = 0; i < MaxItemCount_; i++) {
		items_[i]->Draw(viewProjection);
	}
}

void ItemManager::DrawUI()
{
	for (const std::unique_ptr<Sprite>& sprite : sprites_) {
		sprite->Draw();
	}
}

void ItemManager::SetGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	/*for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		globalVariables->AddItem(groupName_, iInfoNames_[i], iInfo_[i]);
	}*/

	ApplyGlobalVariable();
}

void ItemManager::ApplyGlobalVariable()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < SpriteNames::kSpriteCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}

		sprites_[i]->SetPosition(v2Info_[i][V2ItemNames::kPos]);
	}

	/*for (int i = 0; i < IInfoNames::kIInfoCount; i++) {
		iInfo_[i] = globalVariables->GetIntValue(groupName_, iInfoNames_[i]);
	}*/
}
