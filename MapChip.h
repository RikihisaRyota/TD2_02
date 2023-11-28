#pragma once
#include <d3dx12.h>
#include <wrl.h>

#include <cstdint>
#include <sstream>
#include <vector>
#include <memory>

#include "cVertexPos.h"
#include "cDirectionalLight.h"
#include "cMaterial.h"
#include "Model.h"
#include "ViewProjection.h"
#include "BlockWorldTransform.h"


#include "Collision/Collider.h"
#include "MapChipGraphicsPipeline.h"


const static uint32_t kBlockSize = 2;
const static uint32_t kBlockScreenSize = 40;
const static uint32_t kMaxWidthBlockNum = 400;
const static uint32_t kMaxHeightBlockNum = 36;
const static uint32_t kMaxScreenWidthBlockNum = 32;
const static uint32_t kMaxScreenHeightBlockNum = 18;
const static uint32_t kMaxWidth = kBlockSize * kMaxWidthBlockNum;
const static uint32_t kMaxHeight = kBlockSize * kMaxHeightBlockNum;

class Player;
class MapChip : public Collider{
public:
	struct GPUParam {
		Matrix4x4 mat;
		Vector4 color;
	};
	struct MapChipInstancing {
		uint32_t maxInstance = 10000;
		uint32_t currentInstance;
		// ワールドトランスフォームマトリックスリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuff;
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSRVCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSRVGPUHandle;
		std::vector<std::pair<int,int>> instanceCount;
		GPUParam* gpuPram;
	};
	struct MapColor {
		Vector4 color;
		bool isActive;
	};
	enum Stage
	{
		kStage_1,
		kStage_2,
		kStage_3,
		kStage_4,
		kStage_5,
		kStage_6,
		kStage_7,
		kStage_8,
		kStage_9,
		kStage_10,
		kStage_11,
		kStage_12,
		kStage_13,
		kCount,
	};
	// 前までのBlocksと一緒
	// 名前を変更しただけ
	enum UseBlocks {
		kNone,
		kBlock,
		kRedBlock,
		kItemBlock,
		kNeedleBlock,
		kMarkBlock,

		kUseBlockCount,
	};

	enum InstancingBlocks {
		kBlockNone,
		kBlockDown,
		kBlockDownLeft,
		kBlockDownRight,
		kBlockDownRightLeft,
		kBlockLeftRight,
		kBlockLeft,
		kBlockRight,
		kBlockTop,
		kBlockTopDown,
		kBlockTopDownLeft,
		kBlockTopDownRight,
		kBlockTopLeft,
		kBlockTopRight,
		kBlockTopRightLeft,

		kBlockRedBlock,
		kBlockNeedleBlock,
		kBlockMarkBlock,

		kInstancingBlocksCount,
	};
public:

	void OnCollision() override;
	void SetCollider();

	MapChip();
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);

#pragma region Load,Save
	void LoadCSV();
	void LoadCSV(uint32_t stageNum);
	void LoadCSV(std::string fileName);
	void SaveCSV();
	void SaveCSV(uint32_t stageNum);
	void SaveCSV(std::string fileName);
	void ChangeStage();
#pragma endregion
#pragma region BlockType
	std::vector<std::vector<uint32_t>> GetBlocksTypes() { return map_; }
	const uint32_t GetBlocksType(uint32_t x, uint32_t y) const { return map_[y][x]; }
	const uint32_t GetBlocksType(int x, int y) const { return map_[static_cast<uint32_t>(y)][static_cast<uint32_t>(x)]; }
	const uint32_t GetBlocksType(const Vector3& pos) const { return (map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
	const uint32_t GetBlocksType(const Vector2& pos) const { return(map_[static_cast<uint32_t>(pos.y / kBlockSize)][static_cast<uint32_t>(pos.y / kBlockSize)]); }
#pragma endregion
	void InstancingInitialize();
	void InstancingDraw(const ViewProjection& viewProjection);
	Vector3 GetBlocksCenterWorldPosition(uint32_t x, uint32_t y);
	std::vector<std::vector<BlockWorldTransform>> GetWorldTransforms() {
		return blockWorldTransform_;
	}

	uint32_t GetCurrentStage() { return currentStage_; }
	void SetCurrentStage(uint32_t stageNum);
	void SetBlocks(const Vector3& pos, uint32_t blockType);
	void SetBlocks(const Vector2& pos, uint32_t blockType);
	void SetViewProjection(ViewProjection* viewProjection) { viewProjection_ = viewProjection; }
	void SetPlayer(Player* player) { player_ = player; }
	bool InRange(const Vector3& pos);
	void SetIsClear(bool flag) { isClear_ = flag; }

	void CheckChangeMap();
private:
	int CheckBlock(int y,int x);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	void SetInstancing();
	void SetInstancingBlock(int block, int y, int x);
	void CreateItems();
	void CheckBlockGrit(int y,int x);
	void PlayerTouchBlock();
private:
	// ブロックの種類の最大数
	const uint32_t kMaxTypeBlocks = static_cast<uint32_t>(MapChip::UseBlocks::kUseBlockCount);
	ViewProjection* viewProjection_;
	Player* player_;
	// マップチップの種類
	std::vector<std::vector<uint32_t>> map_;
	std::vector<std::vector<std::vector<uint32_t>>> maps_;
	// ブロックのモデル
	std::vector<Model*> blockModels_;
	// ブロックのワールドトランスフォーム
	std::vector<std::vector<BlockWorldTransform>> blockWorldTransform_;
	std::vector<std::vector<Vector4>> blockColor_;
	// CSVの名前保存
	std::vector<std::string> stageName_;
	// 現在のステージ
	uint32_t currentStage_;

	Vector4 normalColor_;
	Vector4 touchingColor_;
	int playerTouchBlockMinX_;
	int playerTouchBlockMaxX_;
	int playerTouchBlockMinY_;
	int playerTouchBlockMaxY_;
	float delayColor_;

	bool isClear_;

	std::vector<std::vector<uint32_t>> preMap_;
	// インスタンシング描画用
	std::vector<std::unique_ptr<MapChipInstancing>> instancing_;

#pragma region DirectX関連
	// グラフィックパイプライン
	std::unique_ptr<MapChipGraphicsPipeline> basicGraphicsPipeline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<cVertexPos> vertices_;
#pragma endregion
#pragma region インデックスバッファ
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
#pragma endregion
#pragma region マテリアル
	// マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff_;
	// マテリアル
	cMaterial* material_ = nullptr;
#pragma endregion
#pragma region ライティング
	// ライティングリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightBuff_;
	// ライティング
	cDirectionalLight* directionalLight_ = nullptr;
#pragma endregion
#pragma endregion
};

