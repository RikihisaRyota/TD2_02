#include "ParticleManager.h" 

#include "DirectXCommon.h"
#include "TextureManager.h"
#include "MyMath.h"

using namespace Microsoft::WRL;

ParticleManager* ParticleManager::GetInstance() {
	static ParticleManager instance;
	return &instance;
}

void ParticleManager::Update() {
	size_t count = 0;

	for (auto it = instancing_.begin(); it != instancing_.end();) {
		if ((*it)->isAlive_) {
			(*it)->particle->Update();
			(*it)->currentInstance = (*it)->particle->GetAliveParticle();
			for (size_t i = 0; i < (*it)->currentInstance; i++) {
				(*it)->instancingDate[i] = (*it)->particle->GetParticleForGPU(i);
			}
			if (!(*it)->particle->GetIsAlive()) {
				// パーティクルの解放
				//delete (*it)->particle;
				(*it)->isAlive_ = false;
			}
			else {
				++it;
			}
		}
		else {
			break;
		}
	}
	std::sort(instancing_.begin(), instancing_.end(), &ParticleManager::CompareParticles);
}

void ParticleManager::Draw(const ViewProjection& viewProjection) {
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(basicGraphicsPipeline_->GetRootSignature());

	// パイプラインステートの設定
	commandList->SetPipelineState(basicGraphicsPipeline_->GetPipelineState());

	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);

	// インデックスバッファの設定
	commandList->IASetIndexBuffer(&ibView_);

	// CBVをセット（ビュープロジェクション行列）
	commandList->SetGraphicsRootConstantBufferView(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());

	// CBVをセット（Material）
	commandList->SetGraphicsRootConstantBufferView(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::MATERIAL), materialBuff_->GetGPUVirtualAddress());
	for (auto& instancing : instancing_) {
		if (instancing->isAlive_) {

			// instancing用のStructuredBuffをSRVにセット
			commandList->SetGraphicsRootDescriptorTable(static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), instancing->instancingSRVGPUHandle);

			// SRVをセット
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<int>(ParticleGraphicsPipeline::ROOT_PARAMETER_TYP::TEXTURE), instancing->textureHandle);

			// 描画コマンド
			commandList->DrawIndexedInstanced(static_cast<UINT>(indices_.size()), instancing->currentInstance, 0, 0, 0);
		}
	}
}

void ParticleManager::StaticInitialize() {
	// パイプライン生成
	basicGraphicsPipeline_ = std::make_unique<ParticleGraphicsPipeline>();
	basicGraphicsPipeline_->InitializeGraphicsPipeline();
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
	materialBuff_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
	material_->color_ = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	// Lightingを有効化
	material_->enableLightint_ = 0;
	material_->uvTransform_ = MakeAffineMatrix(Vector3(1.0f, 1.0f, 1.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
#pragma endregion
#pragma region インスタンシング生成
	for (size_t i = 0; i < kNumInstancing; i++) {
		auto device = DirectXCommon::GetInstance()->GetDevice();
		Emitter* emitter = new Emitter();
		ParticleMotion* particleMotion = new ParticleMotion();
		Instancing* instancing = new Instancing();


		// パーティクル
		instancing->particle = new Particle();
		instancing->particle->Initialize(emitter, particleMotion);

		instancing->textureHandle = 0;

		instancing->instancingBuff = CreateBuffer(sizeof(ParticleForGPU) * instancing->maxInstance);
		instancing->instancingBuff->Map(0, nullptr, reinterpret_cast<void**>(&instancing->instancingDate));

		// シェーダーリソースビュー
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.NumElements = instancing->maxInstance;
		desc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
		DirectXCommon::GetInstance()->GetSRVCPUGPUHandle(instancing->instancingSRVCPUHandle, instancing->instancingSRVGPUHandle);
		device->CreateShaderResourceView(instancing->instancingBuff.Get(), &desc, instancing->instancingSRVCPUHandle);
		instancing->isAlive_ = false;
		instancing_.emplace_back(instancing);
	}

#pragma endregion

}

void ParticleManager::Initialize() {
	for (auto& instancing : instancing_) {
		if (instancing->isAlive_) {
			instancing->particle->Reset();
		}
	}
}

void ParticleManager::Shutdown() {
	// インスタンシングの解放
	for (auto& instancing : instancing_) {
		// パーティクルの解放
		delete instancing->particle;

		// リソースの解放
		instancing->instancingBuff.Reset();
	}
	materialBuff_.Reset();
	// メンバー変数のリセット
	instancing_.clear();
	vertBuff_.Reset();
	idxBuff_.Reset();
}

ComPtr<ID3D12Resource> ParticleManager::CreateBuffer(UINT size) {
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

void ParticleManager::AddParticle(Emitter* emitter, ParticleMotion* particleMotion,uint32_t textureHandle) {
	for (auto& instancing : instancing_) {
		if (!instancing->isAlive_) {
			instancing->particle->Reset();
			instancing->particle->Initialize(emitter, particleMotion);

			instancing->textureHandle = textureHandle;
			instancing->isAlive_ = true;
			break;
		}
	}
}
