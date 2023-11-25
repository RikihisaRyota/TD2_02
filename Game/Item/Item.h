#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include "Vector3.h"
#include <list>
#include "Model.h"
#include <array>
#include <optional>
#include "Sprite.h"

class Item : public Collider
{
public:
	Item();
	~Item();

	void Init(const Vector3& pos);

	void Update();
	void Draw(const ViewProjection& viewProjection);

	const bool IsLife() const { return isLife_; }

	void SetIsLife(const bool& is) { isLife_ = is; }

private:

	enum class State
	{
		kIsLife,
		kGet,
	};

	void OnCollision() override;

	void SetCollider();

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void IsLifeInit();
	void IsLifeUpdate();

	void GetInit();
	void GetUpdate();

	static void (Item::* spStateInitFuncTable[])();

	static void (Item::* spStateUpdateFuncTable[])();

private:

	State state_ = State::kIsLife;
	std::optional<State> stateRequest_ = std::nullopt;

	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;

	bool isLife_;
	bool isDraw_;

	int countFrame_;

	Vector3 velocity_;

	Vector3 pos_;

	float rotateTheta_;
	float translateTheta_;

	const std::string groupName_ = "Item";

	enum FInfoNames {
		kRotateSpeed,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"通常時の回転速度"
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

	enum IInfoNames {
		kTranslateFrame,
		kIInfoCount,
	};

	std::string iInfoNames_[IInfoNames::kIInfoCount] = {
		"通常時の上下移動一往復のフレーム数",
	};

	std::array<int, IInfoNames::kIInfoCount> iInfo_;

};


class ItemManager
{
public:
	static const uint32_t kMaxItemNum_ = 50;

	static ItemManager* GetInstance();

	void FirstInit();

	void Init();

	void CreateItem(const Vector3& pos);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void DrawUI();

	void AddGetCount();

	const int GetMaxItemNum() const { return MaxItemCount_; }

	const int GetClearItemCountNum() const { return getItemCount_; }

private:
	ItemManager() = default;
	~ItemManager() = default;
	ItemManager(const ItemManager&) = delete;
	const ItemManager& operator=(const ItemManager&) = delete;

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void Clear();

	void SetItem(const Vector3& pos);

	void SetNumTeces();

	void SetSpriteSize();

private:

	int MaxItemCount_;

	int getItemCount_;

	Vector2 itemSize_;
	Vector2 slashSize_;
	Vector2 numSize_;

	static const int MaxDigits = 2;

	enum SpriteNames {
		kItemSprite,
		kSlash,
		kSpriteCount,
	};

	std::string spriteNames_[kSpriteCount] = {
		"アイテムの",
		"スラッシュの"
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

	enum TexColor {
		kBright,
		kDark,
		kColorCount,
	};

	enum DrawNumType {
		kGetItem,
		kMaxItem,
		kNumTypeCount,
	};

	std::array<std::array<std::unique_ptr<Sprite>, MaxDigits>, DrawNumType::kNumTypeCount> numSprites_;

	std::string numItemNames[DrawNumType::kNumTypeCount] = {
		"取得したアイテム数の",
		"全体のアイテム数の",
	};

	std::array<Vector2, DrawNumType::kNumTypeCount> numPoses_;

	std::array<std::array<uint32_t, 10>, TexColor::kColorCount> numTeces_;

	std::array<std::unique_ptr<Item>, kMaxItemNum_> items_;

	const std::string groupName_ = "Item";

	int countFrame_;

	enum FInfoNames {
		kNumericInterval,
		kItemScale,
		kSlashScale_,
		kNumScale,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"数字の間隔",
		"アイテムのスケール",
		"スラッシュのスケール",
		"数字のスケール",
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

	/*enum IInfoNames {
		kCreatIntervalFrame,
		kIInfoCount,
	};

	std::string iInfoNames_[IInfoNames::kIInfoCount] = {
		"生成するまでのインターバルフレーム",
	};

	std::array<int, IInfoNames::kIInfoCount> iInfo_;*/
};
