#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include "Vector3.h"
#include <list>
#include "Model.h"
#include <array>
#include <optional>

class Item : public Collider
{
public:
	Item(const Vector3& pos);
	~Item();

	void Init(const Vector3& pos);

	void Update();
	void Draw(const ViewProjection& viewProjection);

	const bool IsLife() const { return isLife_; }

	void SetIsLife(const bool& is) { isLife_ = is; }

private:

	enum class State
	{
		kCreate,
		kFalling,
	};

	void OnCollision() override;

	void SetCollider();

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void CreateInit();
	void CreateUpdate();

	void FallingInit();
	void FallingUpdate();

	static void (Item::* spStateInitFuncTable[])();

	static void (Item::* spStateUpdateFuncTable[])();

private:

	State state_ = State::kCreate;
	std::optional<State> stateRequest_ = std::nullopt;

	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;

	bool isLife_;

	int countFrame_;

	Vector3 pos_;

	Vector3 velocity_;

	const std::string groupName_ = "Nedle";

	enum FInfoNames {
		kGravity,
		kMaxSpeed,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"重力加速度",
		"最高速度"
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

	enum IInfoNames {
		kCreatFrame,
		kIInfoCount,
	};

	std::string iInfoNames_[IInfoNames::kIInfoCount] = {
		"生成し始めて落ちる始めるまでのフレーム",
	};

	std::array<int, IInfoNames::kIInfoCount> iInfo_;

};


class ItemManager
{
public:
	static ItemManager* GetInstance();

	void Init();

	bool IsCreatNedle();

	void CreateItem(const Vector3& pos);

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	ItemManager() = default;
	~ItemManager() = default;
	ItemManager(const ItemManager&) = delete;
	const ItemManager& operator=(const ItemManager&) = delete;

	void SetGlobalVariable();

	void ApplyGlobalVariable();

	void Clear();

	void SetItem(const Vector3& pos);

private:

	std::list<std::unique_ptr<Item>> items_;

	const std::string groupName_ = "Item";

	int countFrame_;

	enum IInfoNames {
		kCreatIntervalFrame,
		kIInfoCount,
	};

	std::string iInfoNames_[IInfoNames::kIInfoCount] = {
		"生成するまでのインターバルフレーム",
	};

	std::array<int, IInfoNames::kIInfoCount> iInfo_;
};
