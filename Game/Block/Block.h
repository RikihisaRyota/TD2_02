#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Collision/Collider.h"
#include <list>
#include "Model.h"
#include <array>

class OutBlock : public Collider
{
public:
	OutBlock(const Vector3& pos);

	void Update();

private:
	void OnCollision() override;

	void SetCollider();
};

class OutBlockManager 
{
public:
	static OutBlockManager* GetInstance();

	void Init();

	void CreateOutBlock(const Vector3& pos);

	void Update();

private:
	OutBlockManager() = default;
	~OutBlockManager() = default;
	OutBlockManager(const OutBlockManager&) = delete;
	const OutBlockManager& operator=(const OutBlockManager&) = delete;

private:

	std::list<std::unique_ptr<OutBlock>> outBlocks_;
};



class  MarkBlock : public Collider
{
public:
	MarkBlock(const Vector3& pos);

	void Init(const Vector3& pos);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void Reset();

private:
	void OnCollision() override;

	void SetCollider();

	void SetGlobalVariable();

	void ApplyGlobalVariable();

private:

	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;

	int countFrame_;

	bool isSet_;
	bool isChangeScale_;

	Vector3 pos_;

	const std::string groupName_ = "Block";

	enum FInfoNames {
		kMaxScale,
		kFInfoCount,
	};

	std::string fInfoNames_[FInfoNames::kFInfoCount] = {
		"最大の拡大スケール"
	};

	std::array<float, FInfoNames::kFInfoCount> fInfo_;

	enum IInfoNames {
		kBigScaleFrame,
		kSmallScaleFrame,
		kIInfoCount,
	};

	std::string iInfoNames_[IInfoNames::kIInfoCount] = {
		"スケール拡大のフレーム数",
		"スケール縮小のフレーム数"
	};

	std::array<int, IInfoNames::kIInfoCount> iInfo_;
	
};

class  MarkBlockManager
{
public:
	static  MarkBlockManager* GetInstance();

	void Init();

	void CreateMarkBlock(const Vector3& pos);

	void Update();

	void Draw(const ViewProjection& viewProjection);

	void Clear();

private:
	MarkBlockManager() = default;
	~MarkBlockManager() = default;
	MarkBlockManager(const MarkBlockManager&) = delete;
	const MarkBlockManager& operator=(const MarkBlockManager&) = delete;

private:

	std::list<std::unique_ptr<MarkBlock>> markBlocks_;
};


