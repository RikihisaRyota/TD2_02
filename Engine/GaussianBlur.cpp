#include "GaussianBlur.h"

#include <cassert>

#include "DirectXCommon.h"

using namespace Microsoft::WRL;

void GaussianBlur::Initialize(Buffer* buffer, ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState) {
	verticalBlurPipelinePipeline_ = new VerticalBlurPipeline();
	verticalBlurPipelinePipeline_->InitializeGraphicsPipeline();
	horizontalBlurPipeline_ = new HorizontalBlurPipeline();
	horizontalBlurPipeline_->InitializeGraphicsPipeline();
	rootSignature_ = rootSignature;
	pipelineState_ = pipelineState;
	originalBuffer_ = buffer;

	for (size_t i = 0; i < BufferType::kCount; i++) {
		Buffer* buff = new Buffer();
		buffer_.emplace_back(buff);
	}
	CreateResource();

}

void GaussianBlur::Render() {
	SetCommandList();
}

void GaussianBlur::Shutdown() {
	for (auto& buffer : buffer_) {
		buffer->buffer.Reset();
		delete buffer;
	}
	vertBuff_.Reset();
	idxBuff_.Reset();
	constantBuffer_.Reset();
	delete verticalBlurPipelinePipeline_;
	delete horizontalBlurPipeline_;
}

void GaussianBlur::UpdateWeightTable(float blurPower) {
	float total = 0;
	for (uint32_t i = 0; i < kNumWeights; ++i) {
		weights_[i] = std::exp(-0.5f * float(i * i) / blurPower);
		total += 2.0f * weights_[i];
	}
	total = 1.0f / total;
	for (uint32_t i = 0; i < kNumWeights; ++i) {
		weights_[i] *= total;
	}
	// Mapしてデータを転送
	void* mappedData;
	constantBuffer_->Map(0, nullptr, &mappedData);
	memcpy(mappedData, weights_, sizeof(weights_));
	constantBuffer_->Unmap(0, nullptr);
}

void GaussianBlur::CreateResource() {
	HRESULT result = S_FALSE;
	auto common = DirectXCommon::GetInstance();
	auto device = common->GetDevice();
	// レンダーターゲットビューの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	// シェーダーの計算結果をSRGBに変換して書き込む
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//出力結果をSRGBに変換して書き込む
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;//2Dテクスチャとして書き込む
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = 1;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc[BufferType::kCount];
	resourceDesc[BufferType::kVerticalBlur] = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		originalBuffer_->width / 2,
		originalBuffer_->height,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);
	resourceDesc[BufferType::kHorizontalBlur] = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		originalBuffer_->width / 2,
		originalBuffer_->height / 2,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);
	// レンダリング時のクリア値と同じ
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	const wchar_t* kBufferName[] = {
		L"Vertical",
		L"Horizontal"
	};

	// バッファーを作成
	for (size_t i = 0; i < BufferType::kCount; i++) {
		result = device->CreateCommittedResource(
			&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc[i], D3D12_RESOURCE_STATE_COMMON, &clearValue,
			IID_PPV_ARGS(&buffer_.at(i)->buffer));
		assert(SUCCEEDED(result));
		buffer_.at(i)->states = D3D12_RESOURCE_STATE_COMMON;
		buffer_.at(i)->buffer->SetName((std::wstring(L"GaussianBuffer") + kBufferName[i]).c_str());
		buffer_.at(i)->width = uint32_t(resourceDesc[i].Width);
		buffer_.at(i)->height = uint32_t(resourceDesc[i].Height);
		common->GetSRVCPUGPUHandle(buffer_.at(i)->srvCPUHandle, buffer_.at(i)->srvGPUHandle);
		buffer_.at(i)->rtvHandle = common->GetRTVCPUDescriptorHandle();
		device->CreateShaderResourceView(buffer_.at(i)->buffer.Get(), &srvDesc, buffer_.at(i)->srvCPUHandle);
		device->CreateRenderTargetView(buffer_.at(i)->buffer.Get(), &rtvDesc, buffer_.at(i)->rtvHandle);
	}

	// VertexBuffer
	vertices_ = {
		{{-1.0f, -1.0f, -1.0f, 1.0f},{0.0f, 1.0f}}, // 左下
		{{-1.0f, +1.0f, -1.0f, 1.0f},{0.0f, 0.0f}}, // 左上
		{{+1.0f, -1.0f, -1.0f, 1.0f},{1.0f, 1.0f}}, // 右下
		{{+1.0f, +1.0f, -1.0f, 1.0f},{1.0f, 0.0f}}, // 右上
	};

	indices_ = {
		0,  1,  3,
		3,  2,  0,
	};
	UINT sizeVB = static_cast<UINT>(sizeof(VertexPos) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		VertexPos* vertMap = nullptr;
		result = vertBuff_->Map(0, nullptr, reinterpret_cast<void**>(&vertMap));
		if (SUCCEEDED(result)) {
			std::copy(vertices_.begin(), vertices_.end(), vertMap);
			vertBuff_->Unmap(0, nullptr);
		}
	}
	// 頂点バッファビューの作成
	vbView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = sizeVB;
	vbView_.StrideInBytes = sizeof(vertices_[0]);

	// インデックスデータのサイズ
	UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * indices_.size());
	idxBuff_ = CreateBuffer(sizeIB);
	// インデックスバッファへのデータ転送
	uint16_t* indexMap = nullptr;
	result = idxBuff_->Map(0, nullptr, reinterpret_cast<void**>(&indexMap));
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		idxBuff_->Unmap(0, nullptr);
	}
	// インデックスバッファビューの作成
	ibView_.BufferLocation = idxBuff_->GetGPUVirtualAddress();
	ibView_.Format = DXGI_FORMAT_R16_UINT;
	ibView_.SizeInBytes = sizeIB; // 修正: インデックスバッファのバイトサイズを代入

	constantBuffer_ = CreateBuffer(sizeof(weights_));
	UpdateWeightTable(1.0f);
}

void GaussianBlur::SetCommandList() {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	// リソースバリアの変更
	{
		CD3DX12_RESOURCE_BARRIER barrier[] = {
			buffer_.at(kVerticalBlur)->TransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET),
		};
		commandList->ResourceBarrier(_countof(barrier), barrier);
	}
	commandList->OMSetRenderTargets(1, &buffer_.at(kVerticalBlur)->rtvHandle, false, nullptr);
	ClearRenderTarget(buffer_.at(kVerticalBlur)->rtvHandle);
	// ビューポートの設定
	CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(0.0f, 0.0f,FLOAT(buffer_[BufferType::kVerticalBlur]->width),FLOAT(buffer_[BufferType::kVerticalBlur]->height));
	commandList->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, LONG(buffer_[BufferType::kVerticalBlur]->width), LONG(buffer_[BufferType::kVerticalBlur]->height));
	commandList->RSSetScissorRects(1, &rect);

	commandList->SetPipelineState(verticalBlurPipelinePipeline_->GetPipelineState());
	commandList->SetGraphicsRootSignature(verticalBlurPipelinePipeline_->GetRootSignature());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->SetGraphicsRootConstantBufferView(VerticalBlurPipeline::ROOT_PARAMETER_TYP::PRAM, constantBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(VerticalBlurPipeline::ROOT_PARAMETER_TYP::TEXTURE, originalBuffer_->srvGPUHandle);
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);

	// リソースバリアの変更
	{
		CD3DX12_RESOURCE_BARRIER barrier[] = {
			buffer_.at(kHorizontalBlur)->TransitionBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET),
			buffer_.at(kVerticalBlur)->TransitionBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),

		};
		commandList->ResourceBarrier(_countof(barrier), barrier);
	}
	commandList->OMSetRenderTargets(1, &buffer_.at(kHorizontalBlur)->rtvHandle, false, nullptr);
	ClearRenderTarget(buffer_.at(kHorizontalBlur)->rtvHandle);
	// ビューポートの設定
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, FLOAT(buffer_[BufferType::kHorizontalBlur]->width), FLOAT(buffer_[BufferType::kHorizontalBlur]->height));
	commandList->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	rect = CD3DX12_RECT(0, 0, LONG(buffer_[BufferType::kHorizontalBlur]->width), LONG(buffer_[BufferType::kHorizontalBlur]->height));
	commandList->RSSetScissorRects(1, &rect);

	commandList->SetPipelineState(horizontalBlurPipeline_->GetPipelineState());
	commandList->SetGraphicsRootSignature(horizontalBlurPipeline_->GetRootSignature());
	commandList->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->IASetIndexBuffer(&ibView_);
	commandList->SetGraphicsRootConstantBufferView(HorizontalBlurPipeline::ROOT_PARAMETER_TYP::PRAM, constantBuffer_->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(HorizontalBlurPipeline::ROOT_PARAMETER_TYP::TEXTURE, buffer_.at(kVerticalBlur)->srvGPUHandle);
	commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), 1, 0, 0, 0);

	// リソースバリアの変更
	{
		CD3DX12_RESOURCE_BARRIER barrier[] = {
			buffer_.at(kHorizontalBlur)->TransitionBarrier(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE),

		};
		commandList->ResourceBarrier(_countof(barrier), barrier);
	}
}

void GaussianBlur::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE handle) {
	// 全画面のクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	DirectXCommon::GetInstance()->GetCommandList()->ClearRenderTargetView(handle, clearColor, 0, nullptr);
}

Microsoft::WRL::ComPtr<ID3D12Resource> GaussianBlur::CreateBuffer(UINT size) {
	auto device = DirectXCommon::GetInstance()->GetDevice();
	HRESULT result = S_FALSE;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	// バッファ生成
	ComPtr<ID3D12Resource> buffer;
	result = device->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&buffer));
	assert(SUCCEEDED(result));
	return buffer;
}
