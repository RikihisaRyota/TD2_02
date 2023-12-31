#pragma once
#include <d3d12.h>
#include <d3dx12.h>

#include <memory>

#include "BloomPipeline.h"
#include "cBuffer.h"
#include "GaussianBlur.h"
#include "PreBloomPipeline.h"
#include "Vector2.h"
#include "Vector4.h"

class Bloom {
public:
	struct VertexPos {
		Vector4 position{};
		Vector2 texcoord{};
	};

	struct ConstantDate {
		float threshold;
		float knee;
	};
	void Initialize(Buffer* original, Buffer* depth);
	void Update();
	void PreUpdate();
	void Shutdown();

	ID3D12RootSignature* GetRootSignature() { return bloomPipeline_->GetRootSignature(); }
	ID3D12PipelineState* GetPipelineState() { return bloomPipeline_->GetPipelineState(); }
	ID3D12Resource* GetBufferResource() { return temporaryBuffer_->buffer.Get(); }
	Buffer* GetBuffer() { return temporaryBuffer_; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle() { return temporaryBuffer_->rtvHandle; }

private:
	void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void ClearDepthBuffer(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void CreateResource();
	void SetCommandList();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBuffer(UINT size);
	BloomPipeline* bloomPipeline_;
	PreBloomPipeline* preBloomPipeline_;
	// 描画用バッファ
	Buffer* temporaryBuffer_;
	Buffer* originalBuffer_;
	Buffer* originalDepthBuffer_;
	GaussianBlur* gaussianBlur_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_{};
	std::vector<VertexPos> vertices_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> idxBuff_;
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_{};
	std::vector<uint16_t> indices_;
	Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer_;
	ConstantDate* constantDate_;
};