#include "LineGraphicsPipline.h"

#include <cassert>

#include "ConvertString.h"
#include "ShaderCompiler.h"

using namespace Microsoft::WRL;

// 静的メンバ変数の実体化
ID3D12Device* LineGraphicsPipline::sDevice = nullptr;

void LineGraphicsPipline::SetDevice(ID3D12Device* device) {
	assert(device);
	sDevice = device;
}

void LineGraphicsPipline::InitializeGraphicsPipeline() {
	CreateState();
	CreatePSO();
}

void LineGraphicsPipline::CreateState() {
	CreateRootsignature();
	CreateInputLayout();
	CreateBlendState();
	CreateRasiterzerState();
	CreateDepthStencil();
	CreateShaderCompile();
}

void LineGraphicsPipline::CreateRootsignature() {
	HRESULT hr = S_FALSE;
	// Rootsignature設定
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// rootParameterの生成
	D3D12_ROOT_PARAMETER rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::COUNT)] = {};

	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //!< CBVで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL; //!< Shaderで使う
	rootParameters[static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION)].Descriptor.ShaderRegister = static_cast<int>(ROOT_PARAMETER_TYP::VIEWPROJECTION); //!< レジスタ番号1とバインド

	descriptionRootSignature.pParameters = rootParameters; //!< ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //!< 配列の長さ

	// シリアライズしてバイナリする
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	// バイナリを元にRootSignatureを生成
	hr = sDevice->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
	assert(SUCCEEDED(hr));
}

void LineGraphicsPipline::CreateInputLayout() {
	//InputLayout
	inputLayout_[0] =
	{// xyz座標(1行で書いたほうが見やすい)
	 "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
	inputLayout_[1] =
	{// xyz座標(1行で書いたほうが見やすい)
	 "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
	 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	};
}

void LineGraphicsPipline::CreateBlendState() {
	//全ての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
}

void LineGraphicsPipline::CreateRasiterzerState() {
	//裏面（時計回り）を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;
}

void LineGraphicsPipline::CreateShaderCompile() {
	//Shaderをコンパイルする
	vertexShaderBlob_ = ShaderCompiler::Compile(
		L"Resources/Shaders/Line.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = ShaderCompiler::Compile(
		L"Resources/Shaders/Line.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);
}

void LineGraphicsPipline::CreateDepthStencil() {
	// Depthの機能を有効化する
	depthStencilDesc_.DepthEnable = true;
	// 書き込みをする
	depthStencilDesc_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	// 比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc_.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void LineGraphicsPipline::CreatePSO() {
	HRESULT hr = S_FALSE;
	//PSO生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicPipelineStateDesc{};

	// RootSignature
	graphicPipelineStateDesc.pRootSignature = rootSignature_.Get();

	// InputLayout(Descを書かない方法)
	graphicPipelineStateDesc.InputLayout.pInputElementDescs = inputLayout_;
	graphicPipelineStateDesc.InputLayout.NumElements = _countof(inputLayout_);

	// VertexShader
	graphicPipelineStateDesc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};
	// PixelShader
	graphicPipelineStateDesc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};
	// BlendState
	graphicPipelineStateDesc.BlendState = blendDesc_;

	// Rasterizer
	graphicPipelineStateDesc.RasterizerState = rasterizerDesc_;

	// DepthStencilの設定
	graphicPipelineStateDesc.DepthStencilState = depthStencilDesc_;
	graphicPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 書き込むRTVの情報
	graphicPipelineStateDesc.NumRenderTargets = 1;
	graphicPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	// 利用するとポロジ（形状）のタイプ、ライン
	graphicPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

	// どのように画面に色を打ち込むのか設定（気にしなくてもよい）
	graphicPipelineStateDesc.SampleDesc.Count = 1;
	graphicPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// グラフィックスパイプラインの生成
	hr = sDevice->CreateGraphicsPipelineState(&graphicPipelineStateDesc, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}