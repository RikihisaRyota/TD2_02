#include "MapChip.h"

#include <cassert>
#include <fstream>
#include <string>

#include "DirectXCommon.h"
#include "ModelManager.h"
#include "MyMath.h"

#include <Windows.h>

#include "Collision/ColliderShapes/ColliderShapeMapChip2D.h"
#include "Collision/CollisionConfig.h"

#include "WinApp.h"
#include "Collision/CollisionManager.h"
#include "TextureManager.h"
#include "Game/Nedle/Nedle.h"

using namespace Microsoft::WRL;

void MapChip::OnCollision() {}

void MapChip::SetCollider() {
	shapeType_->mapChip2D_.SetMapChip(map_);
	CollisionManager::GetInstance()->SetCollider(this);
}

void MapChip::Update() {

	NedleManager* nedleManager = NedleManager::GetInstance();

	if (nedleManager->IsCreatNedle()) {
		for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
			for (uint32_t x = 0; x < kMaxWidthBlockNum; x++) {
				
				if (map_[y][x] == uint32_t(Blocks::kRedBlock)) {
					nedleManager->CreateNeadle(blockWorldTransform_[y][x]);
				}
			}
		}
	}

	SetCollider();
}

MapChip::MapChip() {

	stageName_ = {
		 "stage_1",
		 "stage_2",
		 "stage_3",
		 "stage_4",
		 "stage_5",
		 "stage_6",
		 "stage_7",
		 "stage_8"
	};

	auto modelManager = ModelManager::GetInstance();
	for (uint32_t i = 0; i < static_cast<uint32_t>(Blocks::kCount) - 1; i++) {
		blockModels_.emplace_back(modelManager->GetBlockModel(i));
	}
	// コンストラクタとInitializeのどっちも呼び出しる
	for (uint32_t stage = 0; stage < Stage::kCount; stage++) {
		maps_.push_back(std::vector<std::vector<uint32_t>>());
		for (uint32_t y = 0; y < kMaxHeightBlockNum; y++) {
			blockWorldTransform_.push_back(std::vector<WorldTransform>());
			maps_[stage].push_back(std::vector<uint32_t>());
			for (uint32_t x = 0; x < kMaxWidthBlockNum; x++) {
				if (stage == 0) {
					blockWorldTransform_[y].push_back(WorldTransform());
					blockWorldTransform_[y][x].Initialize();
					blockWorldTransform_[y][x].translate_ = Vector3(
						float(x * kBlockSize) + float(kBlockSize) * 0.5f,
						float((kMaxHeightBlockNum - y) * kBlockSize) + float(kBlockSize) * 0.5f,
						0.0f
					);
					blockWorldTransform_[y][x].UpdateMatrix();
				}
				maps_[stage][y].push_back(uint32_t());
			}
		}
		LoadCSV(stage);
		currentStage_++;
	}

	currentStage_ = 0;

	shapeType_ = std::make_unique<ColliderShapeMapChip2D>(map_, kMaxHeightBlockNum, Vector3{}, Vector3{ 1.0f, 1.0f, 1.0f });
	shapeType_->SetColliderType(BaseColliderShapeType::ColliderType::RIGID_BODY);
	collisionAttribute_ = 0x00000000;
	collisionMask_ = 0x00000000;

	SetCollisionAttribute(kCollisionAttributeBlock);
	SetCollisionMask(kCollisionAttributePlayer);

	//shapeType_->mapChip2D_.SetNoCollider(0);
	shapeType_->mapChip2D_.SetNoRigitBody(int(Blocks::kBlock));
	shapeType_->mapChip2D_.SetNoRigitBody(int(Blocks::kRedBlock));

	// インスタンシング初期化
	InstancingInitialize();
}

void MapChip::Initialize() {
	map_ = maps_[currentStage_];
}

void MapChip::LoadCSV() {
	LoadCSV(stageName_.at(currentStage_));
}

void MapChip::LoadCSV(uint32_t stageNum) {
	LoadCSV(stageName_.at(stageNum));
}

void MapChip::LoadCSV(std::string fileName) {
	// ファイル名をパスに追加
	fileName = "Resources/CSVs/" + fileName + ".csv";

	// 読み込むファイルを開く
	std::ifstream inputCSVFile(fileName);
	assert(inputCSVFile.is_open());

	// 行ごとに読み込み
	std::string line;
	uint32_t y = 0; // 行数
	while (getline(inputCSVFile, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream lineStream(line);
		uint32_t x = 0; // 列数

		// mapに代入
		std::string cell;
		while (getline(lineStream, cell, ',')) {
			uint32_t value = std::stoi(cell);
			maps_[currentStage_][y][x] = value;

			x++;
		}

		y++;
	}

	// ファイルを閉じる
	inputCSVFile.close();
}

void MapChip::SaveCSV() {
	SaveCSV(stageName_.at(currentStage_));
}

void MapChip::SaveCSV(uint32_t stageNum) {
	SaveCSV(stageName_.at(stageNum));
}

void MapChip::SaveCSV(std::string fileName) {
	// ファイル名をパスに追加
	fileName = "Resources/CSVs/" + fileName + ".csv";

	// 書き込むファイルを開く
	std::ofstream outputCSVFile(fileName);
	assert(outputCSVFile.is_open());

	// copyMap_ の内容をCSVファイルに書き込む
	for (int y = 0; y < kMaxHeightBlockNum; ++y) {
		for (int x = 0; x < kMaxWidthBlockNum; ++x) {
			// コンマ区切りでデータを書き込む
			maps_[currentStage_][y][x] = map_[y][x];
			outputCSVFile << map_[y][x];
			outputCSVFile << ",";
		}
		// 行末に改行を挿入
		outputCSVFile << std::endl;
	}

	// ファイルを閉じる
	outputCSVFile.close();
}

void MapChip::ChangeStage() {
	map_ = maps_[currentStage_];
}

void MapChip::Draw(const ViewProjection& viewProjection) {
	InstancingDraw(viewProjection);
}

Vector3 MapChip::GetBlocksCenterWorldPosition(uint32_t x, uint32_t y) {
	return MakeTranslate(blockWorldTransform_[y][x].matWorld_);
}
void MapChip::InstancingInitialize() {
	// パイプライン生成
	basicGraphicsPipeline_ = std::make_unique<MapChipGraphicsPipeline>();
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
#pragma region ライティングバッファ
	directionalLightBuff_ = CreateBuffer(sizeof(cDirectionalLight));
	// ライティングバッファへのデータ転送
	result = directionalLightBuff_->Map(0, nullptr, reinterpret_cast<void**>(&directionalLight_));
	// 初期値代入
	directionalLight_->color_ = { 1.0f,1.0f,1.0f,1.0f };
	directionalLight_->direction_ = { 0.5f,-0.7f,1.0f };
	directionalLight_->intensiy_ = 1.0f;
	directionalLight_->sharpness_ = 1.0f;
#pragma endregion
#pragma region インスタンシング生成
	for (size_t i = 0; i < size_t(MapChip::Blocks::kCount) - 1; i++) {
		auto device = DirectXCommon::GetInstance()->GetDevice();
		MapChipInstancing* instancing = new MapChipInstancing();
		instancing->instancingBuff = CreateBuffer(sizeof(Matrix4x4) * instancing->maxInstance);
		instancing->instancingBuff->Map(0, nullptr, reinterpret_cast<void**>(&instancing->mat));
		// シェーダーリソースビュー
		D3D12_SHADER_RESOURCE_VIEW_DESC desc{};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Buffer.NumElements = instancing->maxInstance;
		desc.Buffer.StructureByteStride = sizeof(Matrix4x4);
		DirectXCommon::GetInstance()->GetSRVCPUGPUHandle(instancing->instancingSRVCPUHandle, instancing->instancingSRVGPUHandle);
		device->CreateShaderResourceView(instancing->instancingBuff.Get(), &desc, instancing->instancingSRVCPUHandle);
		instancing_.emplace_back(instancing);
	}
}
void MapChip::InstancingDraw(const ViewProjection& viewProjection) {
	for (auto& instancing : instancing_) {
		instancing->currentInstance = 0;
	}
	float ratio = std::tanf(viewProjection.fovAngleY_ / 2) * (blockWorldTransform_[0][0].translate_.z - viewProjection.translate_.z) * 2;

	int32_t yNum = static_cast<int32_t>(ratio / int32_t(kBlockSize)) + 1;
	int32_t xNum = static_cast<int32_t>(ratio * viewProjection.aspectRatio_ / int32_t(kBlockSize)) + 1;

	int32_t xMin = int32_t(int32_t(viewProjection.translate_.x) / int32_t(kBlockSize) - xNum / 2) - 1;
	xMin = std::clamp(xMin, 0, int32_t(kMaxWidthBlockNum));
	int32_t xMax = int32_t(int32_t(viewProjection.translate_.x) / int32_t(kBlockSize) + xNum / 2) + 1;
	xMax = std::clamp(xMax, 0, int32_t(kMaxWidthBlockNum));
	int32_t yMin = int32_t(int32_t(viewProjection.translate_.y) / int32_t(kBlockSize) + yNum / 2) + 1;
	yMin = kMaxHeightBlockNum - yMin;
	yMin = std::clamp(yMin, 0, int32_t(kMaxHeightBlockNum));
	int32_t yMax = int32_t(int32_t(viewProjection.translate_.y) / int32_t(kBlockSize) - yNum / 2) - 1;
	yMax = kMaxHeightBlockNum - yMax;
	yMax = std::clamp(yMax, 0, int32_t(kMaxHeightBlockNum));

	for (int32_t y = yMin; y < yMax; y++) {
		for (int32_t x = xMin; x < xMax; x++) {
			auto block = map_[y][x];
			switch (block) {
			case uint32_t(Blocks::kBlock):
				instancing_.at(uint32_t(Blocks::kBlock) - 1)->mat[instancing_.at(uint32_t(Blocks::kBlock) - 1)->currentInstance] = blockWorldTransform_.at(y).at(x).matWorld_;
				instancing_.at(uint32_t(Blocks::kBlock) - 1)->currentInstance++;
				break;
			case uint32_t(Blocks::kRedBlock):
				instancing_.at(uint32_t(Blocks::kRedBlock) - 1)->mat[instancing_.at(uint32_t(Blocks::kRedBlock) - 1)->currentInstance] = blockWorldTransform_.at(y).at(x).matWorld_;
				instancing_.at(uint32_t(Blocks::kRedBlock) - 1)->currentInstance++;
				break;
			default:
				break;
			}
		}
	}
	auto commandList = DirectXCommon::GetInstance()->GetCommandList();
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(basicGraphicsPipeline_->GetRootSignature());

	// パイプラインステートの設定
	commandList->SetPipelineState(basicGraphicsPipeline_->GetPipelineState());

	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// CBVをセット（ビュープロジェクション行列）
	commandList->SetGraphicsRootConstantBufferView(static_cast<int>(MapChipGraphicsPipeline::ROOT_PARAMETER_TYP::VIEWPROJECTION), viewProjection.constBuff_->GetGPUVirtualAddress());
	for (uint32_t i=0; auto & instancing : instancing_) {
		if (instancing->mat!=nullptr) {
			// 頂点バッファの設定
			commandList->IASetVertexBuffers(0, 1, blockModels_.at(i)->GetMesh(0)->GetVBView());

			// CBVをセット（Material）
			commandList->SetGraphicsRootConstantBufferView(static_cast<int>(MapChipGraphicsPipeline::ROOT_PARAMETER_TYP::MATERIAL), blockModels_.at(i)->GetMaterial(0)->GetMaterialBuff()->GetGPUVirtualAddress());

			// DirectionalLight用のCBufferの場所を設定
			commandList->SetGraphicsRootConstantBufferView(static_cast<int>(MapChipGraphicsPipeline::ROOT_PARAMETER_TYP::LIGHTING), blockModels_.at(i)->GetMaterial(0)->GeLightingBuff()->GetGPUVirtualAddress());

			// instancing用のStructuredBuffをSRVにセット
			commandList->SetGraphicsRootDescriptorTable(static_cast<int>(MapChipGraphicsPipeline::ROOT_PARAMETER_TYP::WORLDTRANSFORM), instancing->instancingSRVGPUHandle);

			// SRVをセット
			TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(commandList, static_cast<int>(MapChipGraphicsPipeline::ROOT_PARAMETER_TYP::TEXTURE), blockModels_.at(i)->GetMaterial(0)->GetTextureHandle());

			// 描画コマンド
			commandList->DrawInstanced(static_cast<UINT> (blockModels_.at(i++)->GetMesh(0)->GetVertices().size()), instancing->currentInstance, 0, 0);
		}
	}

}

bool MapChip::InRange(const Vector3& pos) {
	if (pos.x < 0.0f || pos.x > float(kMaxWidth)) {
		return false;
	}
	if (pos.y < 0.0f || pos.y > float(kMaxHeight)) {
		return false;
	}
	return true;
}

void MapChip::SetCurrentStage(uint32_t stageNum)
{
	if (stageNum >= Stage::kCount) {
		stageNum = Stage::kCount - 1;
		currentStage_ = stageNum;
	}
	else {
		currentStage_ = stageNum;
	}
}

void MapChip::SetBlocks(const Vector3& pos, uint32_t blockType) {
	int32_t x = int32_t(pos.x / float(kBlockSize));
	int32_t y = kMaxHeightBlockNum - int32_t(pos.y / float(kBlockSize));
	map_[y][x] = blockType;
}

void MapChip::SetBlocks(const Vector2& pos, uint32_t blockType) {
	uint32_t x = uint32_t(pos.x / kBlockScreenSize);
	// カメラの初期値
	float cameraPosX = viewProjection_->kInitializeTranslate_.x;
	uint32_t differenceX = uint32_t((viewProjection_->translate_.x - cameraPosX) / float(kBlockSize));
	// 697
	uint32_t y = uint32_t(pos.y / kBlockScreenSize);
	float cameraPosY = viewProjection_->kInitializeTranslate_.y;
	uint32_t differenceY = uint32_t((viewProjection_->translate_.y - cameraPosY) / float(kBlockSize));
	differenceY = kMaxScreenHeightBlockNum - differenceY;
	if (y + differenceY < kMaxHeightBlockNum &&
		x + differenceX < kMaxWidthBlockNum) {
		map_[y + differenceY][x + differenceX] = blockType;
	}
}

ComPtr<ID3D12Resource> MapChip::CreateBuffer(UINT size) {
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