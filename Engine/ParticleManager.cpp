#include "ParticleManager.h"

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "MyMath.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* ParticleManager::sDevice = nullptr;
Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> ParticleManager::cmdList_;

void ParticleManager::SetDevice(ID3D12Device* device) {
	// nullptrチェック
	assert(device);

	sDevice = device;
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* cmdList) {
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(cmdList_ == nullptr);
	// コマンドリストをセット
	cmdList_ = cmdList;
}
void ParticleManager::PostDraw() {
	// コマンドリストの解除
	cmdList_ = nullptr;
}

ParticleManager* ParticleManager::Create() {
	// Particleのインスタンスを生成
	ParticleManager* particle = new ParticleManager();
	assert(particle);
	// パイプライン初期化
	particle->basicGraphicsPipeline_ = std::make_unique<ParticleGraphicsPipeline>();

	particle->basicGraphicsPipeline_->InitializeGraphicsPipeline();
	// 初期化
	particle->Initialize();
	return particle;
}

void ParticleManager::Update() {
}

void ParticleManager::Draw(const ViewProjection& viewProjection, uint32_t textureHadle) {
	BasicDraw( viewProjection, textureHadle);
}

void ParticleManager::SetMaterial(const cMaterial& material) {
	material_->color_ = material.color_;
	material_->enableLightint_ = material.enableLightint_;
	material_->uvTransform_ = material.uvTransform_;
}


void ParticleManager::Initialize() {
	HRESULT result = S_FALSE;

	vertices_ = {
		//	x      y     z      w      nx    ny    nz     u     v
		{{-1.0f, -1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{0.0f, 1.0f}}, // 左下 0
		{{-1.0f, +1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{0.0f, 0.0f}}, // 左上 1
		{{+1.0f, +1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{1.0f, 0.0f}}, // 右上 2
		{{+1.0f, -1.0f, 0.0f, +1.0f},{0.0f, 0.0f,-1.0f},{1.0f, 1.0f}}, // 右下 3
	};
	// 頂点インデックスの設定
	indices_ = { 0, 1, 2,
				 0, 2, 3,
	};
#pragma region 頂点バッファ
	// 頂点データのサイズ
	UINT sizeVB = static_cast<UINT>(sizeof(cVertexPos) * vertices_.size());
	vertBuff_ = CreateBuffer(sizeVB);
	// 頂点バッファへのデータ転送
	{
		cVertexPos* vertMap = nullptr;
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
#pragma endregion 頂点バッファ
#pragma region インデックスバッファ
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
#pragma endregion インデックスバッファ
#pragma region マテリアルバッファ
	materialBuff_ = CreateBuffer(sizeof(cMaterial));
	// マテリアルへのデータ転送
	result = materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = 0;
	material_->uvTransform_ = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
#pragma endregion
#pragma region ワールドトランスフォームバッファ
	instancingBuff_ = CreateBuffer(sizeof(TransformationMatrix) * kNumInstance_);
	instancingBuff_->Map(0, nullptr,reinterpret_cast<void**>(&instancingDate_));
	// 単位行列
	for (size_t i = 0; i < kNumInstance_; i++) {
		instancingDate_[i].World = MakeIdentity4x4();
		instancingDate_[i].World.m[3][0] = i * 0.1f;
		instancingDate_[i].World.m[3][1] = i * 0.1f;
		instancingDate_[i].World.m[3][2] = i * 0.1f;
	}
	// シェーダーリソースビュー
	D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	desc.Buffer.NumElements = kNumInstance_;
	desc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	descriptorSizeSRV = sDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	DirectXCommon::GetInstance()->GetCPUGPUHandle(instancingSRVCPUHandle_, instancingSRVGPUHandle_);
	sDevice->CreateShaderResourceView(instancingBuff_.Get(),&desc, instancingSRVCPUHandle_);
#pragma endregion
}

void ParticleManager::BasicDraw(const ViewProjection& viewProjection, uint32_t textureHadle) {
	// ルートシグネチャの設定
	cmdList_->SetGraphicsRootSignature(basicGraphicsPipeline_->GetRootSignature());

	// パイプラインステートの設定
	cmdList_->SetPipelineState(basicGraphicsPipeline_->GetPipelineState());

	// プリミティブ形状を設定
	cmdList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	cmdList_->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	cmdList_->IASetIndexBuffer(&ibView_);

	// instancing用のStructuredBuffをSRVにセット
	cmdList_->SetGraphicsRootDescriptorTable(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), instancingSRVGPUHandle_);
	
	// CBVをセット（ビュープロジェクション行列）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	cmdList_->SetGraphicsRootConstantBufferView(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(cmdList_.Get(), static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::TEXTURE), textureHadle);

	// 描画コマンド
	cmdList_->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), kNumInstance_, 0, 0, 0);
}

ComPtr<ID3D12Resource> ParticleManager::CreateBuffer(UINT size) {
	HRESULT result = S_FALSE;
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	// バッファ生成
	ComPtr<ID3D12Resource> buffer;
	result = sDevice->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&buffer));
	assert(SUCCEEDED(result));
	return buffer;
}