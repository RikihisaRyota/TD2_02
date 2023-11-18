#include "DirectXCommon.h"
#include <algorithm>
#include <wrl/client.h>
#include <cassert>

#include "ConvertString.h"
#include "TextureManager.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

using namespace Microsoft::WRL;

DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instans;
	return &instans;
}

void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {

	// nullptrチェック
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	winApp_ = winApp;
	backBufferWidth_ = backBufferWidth;
	backBufferHeight_ = backBufferHeight;

	// DXGIデバイスの初期化
	InitializeDXGIDevice();

	// コマンド関連初期化
	InitializeCommand();

	// swapChainの生成
	CreateSwapChain();

	// レンダーターゲットの作成
	CreateRenderTargets();

	// 深度バッファの生成
	CreateDepthBuffer();

	// フェンス生成
	CreateFence();

	// ポストエフェクトの初期化
	PostEffectInitialize();

	// ブルームの初期化
	BloomInitialize();
}

void DirectXCommon::Update() {
	bloom_->PreUpdate();
}

void DirectXCommon::PreDraw() {
	// セットするために呼び出すのは効率が悪い
	ID3D12DescriptorHeap* ppHeaps[] = { srvDescriptorHeap_.Get() };
	commandList_->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	// リソースバリアを変更(表示状態->描画対象)
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		mainBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = mainBuffer_->rtvHandle;
	commandList_->OMSetRenderTargets(1, &rtvHandle, false, &mainDepthBuffer_->dpsCPUHandle);

	// 全画面クリア
	ClearRenderTarget();
	// 深度バッファクリア
	ClearDepthBuffer();

	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
		CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight);
	commandList_->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight);
	commandList_->RSSetScissorRects(1, &rect);
}

void DirectXCommon::PostDraw() {
	HRESULT hr = S_FALSE;
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();


	bloom_->Update();
	// リソースバリアの変更
	CD3DX12_RESOURCE_BARRIER barrier[2];
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers_[bbIndex]->buffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		mainBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList_->ResourceBarrier(2, barrier);
	commandList_->OMSetRenderTargets(1, &backBuffers_[bbIndex]->rtvHandle, false, &mainDepthBuffer_->dpsCPUHandle);

	postEffect_->Update();

	// リソースバリアの変更
	barrier[0] = CD3DX12_RESOURCE_BARRIER::Transition(
		mainBuffer_->buffer.Get(),
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
		D3D12_RESOURCE_STATE_PRESENT);

	barrier[1] = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers_[bbIndex]->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	commandList_->ResourceBarrier(2, barrier);
}

void DirectXCommon::PreUIDraw() {
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();
	// リソースバリアを変更(表示状態->描画対象)
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers_[bbIndex]->buffer.Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	// TransitionBarrierを張る
	commandList_->ResourceBarrier(1, &barrier);
	commandList_->OMSetRenderTargets(1, &backBuffers_[bbIndex]->rtvHandle, false, &mainDepthBuffer_->dpsCPUHandle);

	// ビューポートの設定
	CD3DX12_VIEWPORT viewport =
		CD3DX12_VIEWPORT(0.0f, 0.0f, WinApp::kWindowWidth, WinApp::kWindowHeight);
	commandList_->RSSetViewports(1, &viewport);
	// シザリング矩形の設定
	CD3DX12_RECT rect = CD3DX12_RECT(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight);
	commandList_->RSSetScissorRects(1, &rect);
}

void DirectXCommon::PostUIDraw() {
	HRESULT hr = S_FALSE;
	UINT bbIndex = swapChain_->GetCurrentBackBufferIndex();

	// リソースバリアの変更(コピー先->描画)
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers_[bbIndex]->buffer.Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);

	commandList_->ResourceBarrier(1, &barrier);

	WaitForGPU();
}

void DirectXCommon::ClearRenderTarget() {
	// 全画面のクリア
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	commandList_->ClearRenderTargetView(mainBuffer_->rtvHandle, clearColor, 0, nullptr);
}

void DirectXCommon::ClearDepthBuffer() {
	// 深度バッファのクリア
	commandList_->ClearDepthStencilView(mainDepthBuffer_->dpsCPUHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr = S_FALSE;

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバックレイヤーを有効化する
		debugController->EnableDebugLayer();
		//更にGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	// 対応レベルの配列
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	// DXGIファクトリーの生成
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	// アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter4>> adapters;
	// ここに特定の名前を持つアダプターオブジェクトが入る
	ComPtr<IDXGIAdapter4> tmpAdapter;
	// パフォーマンスが高いものから順に、全てのアダプターを列挙する
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&tmpAdapter)) !=
		DXGI_ERROR_NOT_FOUND;
		i++) {
		// 動的配列に追加する
		adapters.push_back(tmpAdapter);
	}

	// ハードウェアアダプタを優先的に処理
	std::stable_sort(
		adapters.begin(), adapters.end(),
		[](const ComPtr<IDXGIAdapter4>& lhs, const ComPtr<IDXGIAdapter4>& rhs) {
			DXGI_ADAPTER_DESC3 lhsDesc;
			lhs->GetDesc3(&lhsDesc); // アダプターの情報を取得
			DXGI_ADAPTER_DESC3 rhsDesc;
			rhs->GetDesc3(&rhsDesc); // アダプターの情報を取得
			return (lhsDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE) <
				(rhsDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE);
		});
	hr = S_FALSE;
	D3D_FEATURE_LEVEL featureLevel;
	for (int i = 0; i < adapters.size(); i++) {
		// デバイスを生成
		for (int levelIndex = 0; levelIndex < _countof(levels); levelIndex++) {
			hr = D3D12CreateDevice(adapters[i].Get(), levels[levelIndex], IID_PPV_ARGS(&device_));
			if (SUCCEEDED(hr)) {
				// デバイスを生成できた時点でループを抜ける
				featureLevel = levels[levelIndex];
				break;
			}
		}
		// このアダプタで生成できてたら完了
		if (SUCCEEDED(hr)) {
			break;
		}
	}

	assert(!!device_);
	assert(SUCCEEDED(hr));

	Log("Complete create D3D12Device!!\n");//初期化完了のログをだす

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバックレイヤーの相互作用バグによるエラーメッセージ
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
		//解放
		infoQueue->Release();
	}
#endif // _DEBUG
}

void DirectXCommon::InitializeCommand() {
	HRESULT hr = S_FALSE;
	// コマンドリストキューを作成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr = device_->CreateCommandQueue(
		&commandQueueDesc,
		IID_PPV_ARGS(&commandQueue_));
	assert(SUCCEEDED(hr));

	// コマンドアロケータを作成
	hr = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&commandAllocator_));
	assert(SUCCEEDED(hr));

	// コマンドリストの作成
	hr = device_->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		commandAllocator_.Get(),
		nullptr,
		IID_PPV_ARGS(&commandList_));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateSwapChain() {
	HRESULT hr = S_FALSE;

	// スワップチェーンの生成
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = backBufferWidth_;//画面の幅。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Height = backBufferHeight_;//画面の高さ。ウィンドウのクライアント領域を同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに写したら、中身を破棄
	ComPtr<IDXGISwapChain1> swapChain1;
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	hr = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(),
		winApp_->GetHwnd(),
		&swapChainDesc,
		nullptr,
		nullptr,
		&swapChain1);
	assert(SUCCEEDED(hr));


	// swapChain4を得る
	swapChain1->QueryInterface(IID_PPV_ARGS(&swapChain_));
	assert(SUCCEEDED(hr));
}

void DirectXCommon::CreateRenderTargets() {
	HRESULT hr = S_FALSE;

	// デスクリプタサイズを取得
	RTVDescriptorHandleIncrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	SRVDescriptorHandleIncrementSize = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	mainDepthBuffer_ = new Buffer();
	mainBuffer_ = new Buffer();

	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	hr = swapChain_->GetDesc(&swcDesc);
	assert(SUCCEEDED(hr));

	// デスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeapDesc{};
	rtvDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;// レンダーターゲットビュー用
	rtvDescriptorHeapDesc.NumDescriptors = 32;
	hr = device_->CreateDescriptorHeap(&rtvDescriptorHeapDesc, IID_PPV_ARGS(&rtvDescriptorHeap_));
	assert(SUCCEEDED(hr));
	// デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // シェーダから見えるように
	descHeapDesc.NumDescriptors = kNumDescriptors; // シェーダーリソースビュー1つ
	auto result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&srvDescriptorHeap_)); // 生成
	assert(SUCCEEDED(result));

	// 表裏の2つ分について
	backBuffers_.resize(swcDesc.BufferCount);
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
	for (int i = 0; i < backBuffers_.size(); i++) {
		backBuffers_[i] = new Buffer();
		// スワップチェーンからバッファを取得
		hr = swapChain_->GetBuffer(i, IID_PPV_ARGS(&backBuffers_[i]->buffer));
		backBuffers_[i]->buffer->SetName((L"SwapChainBuffer" + std::to_wstring(i)).c_str());
		assert(SUCCEEDED(hr));
		backBuffers_[i]->rtvHandle = GetRTVCPUDescriptorHandle();
		// レンダーターゲットビューの生成
		device_->CreateRenderTargetView(backBuffers_[i]->buffer.Get(), &rtvDesc, backBuffers_[i]->rtvHandle);
	}
	// ヒーププロパティ
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// リソース設定
	CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		WinApp::kWindowWidth,
		WinApp::kWindowHeight,
		1, 0, 1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_TEXTURE_LAYOUT_UNKNOWN, 0);
	// レンダリング時のクリア値と同じ
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	D3D12_CLEAR_VALUE clearValue = CD3DX12_CLEAR_VALUE(
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	result = device_->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_PRESENT, &clearValue,
		IID_PPV_ARGS(&mainBuffer_->buffer));
	assert(SUCCEEDED(result));
	mainBuffer_->buffer->SetName(L"mainBuffer");
	GetSRVCPUGPUHandle(mainBuffer_->srvCPUHandle, mainBuffer_->srvGPUHandle);
	mainBuffer_->rtvHandle = GetRTVCPUDescriptorHandle();
	device_->CreateShaderResourceView(mainBuffer_->buffer.Get(), &srvDesc, mainBuffer_->srvCPUHandle);
	device_->CreateRenderTargetView(mainBuffer_->buffer.Get(), &rtvDesc, mainBuffer_->rtvHandle);
}

void DirectXCommon::CreateDepthBuffer() {
	// DepthStemcilTextureをウィンドウのサイズで作成
	mainDepthBuffer_->buffer = CreateDepthStencilTextureResource(WinApp::kWindowWidth, WinApp::kWindowHeight);
	mainDepthBuffer_->buffer->SetName(L"mainDepthBuffer");
	// DSV用のヒープでディスクリプタの数は1。DSVはShader内で触るものではないにで、ShaderVisibleはfalse
	dsvHeap_ = CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2DTexture
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	mainDepthBuffer_->dpsCPUHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	// DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(mainDepthBuffer_->buffer.Get(), &dsvDesc, mainDepthBuffer_->dpsCPUHandle);
}

void DirectXCommon::CreateFence() {
	HRESULT hr = S_FALSE;

	hr = device_->CreateFence(
		fenceValue_,
		D3D12_FENCE_FLAG_NONE,
		IID_PPV_ARGS(&fence_)
	);
	assert(SUCCEEDED(hr));
}

void DirectXCommon::PostEffectInitialize() {
	postEffect_ = new PostEffect();
	postEffect_->Initialize(mainBuffer_);
}

void DirectXCommon::BloomInitialize() {
	bloom_ = new Bloom();
	bloom_->Initialize(mainBuffer_, mainDepthBuffer_);
}

void DirectXCommon::WaitForGPU() {
	// 命令のクローズ
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));

	// GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* cmdLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, cmdLists);

	// GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	// コマンドリストの実行完了を待つ
	commandQueue_->Signal(fence_.Get(), ++fenceValue_);
	if (fence_->GetCompletedValue() != fenceValue_) {
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence_->SetEventOnCompletion(fenceValue_, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	// コマンドリストのリセット
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}


ComPtr<ID3D12Resource> DirectXCommon::CreateDepthStencilTextureResource(int32_t width, int32_t height) {
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;// Textureの幅
	resourceDesc.Height = height;// Textureの高さ
	resourceDesc.MipLevels = 1;// mipmapの数
	resourceDesc.DepthOrArraySize = 1;// 奥行き or 配列Textureの配列数
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;// DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1;// サンプリングカウント。1固定
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;// 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;// DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;// VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;// 1.0f(最大値)でクリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる
	ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties, // Heapの設定
		D3D12_HEAP_FLAG_NONE, // Heapの特殊な設定
		&resourceDesc, // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue, // Clear最適値
		IID_PPV_ARGS(&resource));

	assert(SUCCEEDED(hr));

	return resource;
}

Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> DirectXCommon::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
	ComPtr<ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	return descriptorHeap;
}

void DirectXCommon::Release() {
	// Direct3D関連
	// 描画関連
	fence_.Reset();
	// 深度バッファ関連
	mainDepthBuffer_->buffer.Reset();
	delete mainDepthBuffer_;
	dsvHeap_.Reset();
	// レンダーターゲット関連
	rtvDescriptorHeap_.Reset();
	for (auto& ite : backBuffers_) {
		ite->buffer.Reset();
	}
	// シェーダーリソース
	srvDescriptorHeap_.Reset();
	// スワップチェーン関連
	swapChain_.Reset();
	// ポストエフェクト
	postEffect_->Shutdown();
	delete postEffect_;
	// ガウシアンブラー
	bloom_->Shutdown();
	delete bloom_;
	// コマンド関連
	commandQueue_.Reset();
	commandAllocator_.Reset();
	commandList_.Reset();
	mainBuffer_->buffer.Reset();
	delete mainBuffer_;
	// DXGI関連
	device_.Reset();
	dxgiFactory_.Reset();
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVCPUDescriptorHandle() {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (SRVDescriptorHandleIncrementSize * numSRVDescriptorsCount);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DirectXCommon::GetSRVGPUDescriptorHandle() {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (SRVDescriptorHandleIncrementSize * numSRVDescriptorsCount);
	return handleGPU;
}

void DirectXCommon::GetSRVCPUGPUHandle(D3D12_CPU_DESCRIPTOR_HANDLE& cpu, D3D12_GPU_DESCRIPTOR_HANDLE& gpu) {
	cpu = GetSRVCPUDescriptorHandle();
	gpu = GetSRVGPUDescriptorHandle();
	numSRVDescriptorsCount++;
}

D3D12_CPU_DESCRIPTOR_HANDLE DirectXCommon::GetRTVCPUDescriptorHandle() {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = rtvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (RTVDescriptorHandleIncrementSize * numRTVDescriptorsCount);
	numRTVDescriptorsCount++;
	return handleCPU;

}