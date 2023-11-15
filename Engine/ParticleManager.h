#pragma once

#include <d3dx12.h>
#include <vector>

#include "cMaterial.h"
#include "cVertexPos.h"
#include "Particle.h"
#include "ParticleGraphicsPipline.h"
#include "ParticleShaderStruct.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class ParticleManager {
public:
	struct Instancing {
		uint32_t textureHandle;
		Particle* particle;
		uint32_t maxInstance = 1024;
		uint32_t currentInstance;
		// ワールドトランスフォームマトリックスリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuff;
		// ワールドトランスフォーム
		ParticleForGPU* instancingDate = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSRVCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSRVGPUHandle;
		// マテリアルリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> materialBuff;
		// マテリアル
		cMaterial* material = nullptr;
	};
public:
	static ParticleManager* GetInstance();
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
	void Shutdown();
	void AddParticle(Emitter* emitter, ParticleMotion* particleMotion, uint32_t textureHandle);
private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
#pragma region DirectX関連
	// グラフィックパイプライン
	ParticleGraphicsPipeline* basicGraphicsPipeline_ = nullptr;
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
#pragma region 
	std::vector<Instancing*> instancing_;
#pragma endregion
	
};
