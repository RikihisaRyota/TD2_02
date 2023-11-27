#pragma once

#include <d3dx12.h>
#include <vector>

#include "Particle.h"
#include "ParticleUIGraphicsPipeline.h"
#include "ParticleShaderStruct.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "ParticleUI.h"

class ParticleUIManager {
	struct  VertexPosUv {
		Vector4 pos;
		Vector2 uv;
	};

	struct ConstBufferData {
		Vector4 color; // 色 (RGBA)
		Matrix4x4 mat;   // ３Ｄ変換行列
	};

public:
	struct Instancing {
		uint32_t textureHandle;
		ParticleUI* particle;
		uint32_t maxInstance = 10000;
		uint32_t currentInstance;
		// ワールドトランスフォームマトリックスリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingBuff;
		// ワールドトランスフォーム
		ParticleForGPU* instancingDate = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE instancingSRVCPUHandle;
		D3D12_GPU_DESCRIPTOR_HANDLE instancingSRVGPUHandle;
		bool isAlive_;
	};
public:
	static ParticleUIManager* GetInstance();
	void StaticInitialize();
	void Initialize();
	void Update();
	void Draw();
	void Shutdown();
	void AddParticle(Emitter* emitter, ParticleMotion* particleMotion, uint32_t textureHandle);
private:
	static const size_t kNumInstancing = 100;
	static bool CompareParticles(const Instancing* a, const Instancing* b) {
		return a->isAlive_ > b->isAlive_;
	}
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
#pragma region DirectX関連
	// グラフィックパイプライン
	std::unique_ptr<ParticleUIGraphicsPipeline> basicGraphicsPipeline_ = nullptr;
#pragma region 頂点バッファ
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	// 頂点データ配列
	std::vector<VertexPosUv> vertices_;
#pragma endregion
#pragma region インデックスバッファ
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	// 頂点インデックスデータ
	std::vector<uint16_t> indices_;
#pragma endregion
	std::vector<Instancing*> instancing_;

};

