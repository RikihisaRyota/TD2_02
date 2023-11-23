#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include <list>
#include "Model.h"
#include <array>
#include <optional>

class Nedle : public Collider
{
public:
	Nedle(const Vector3& position);
	~Nedle();

	void Update();
	void Draw(const ViewProjection& viewProjection);

	const bool IsLife() const { return isLife_; }

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

	static void (Nedle::* spStateInitFuncTable[])();

	static void (Nedle::* spStateUpdateFuncTable[])();

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


class NedleManager
{
public:
	static NedleManager* GetInstance();

	void Init();

	bool IsCreatNedle();

	void CreateNeadle(const Vector3& position);

	void Clear();

	void Update();

	void Draw(const ViewProjection& viewProjection);

private:
	NedleManager() = default;
	~NedleManager() = default;
	NedleManager(const NedleManager&) = delete;
	const NedleManager& operator=(const NedleManager&) = delete;

	void SetGlobalVariable();

	void ApplyGlobalVariable();

private:

	std::list<std::unique_ptr<Nedle>> nedles_;

	const std::string groupName_ = "Nedle";

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
