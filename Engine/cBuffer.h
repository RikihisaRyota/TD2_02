#pragma once

#include <d3d12.h>
#include <d3dx12.h>

#include <memory>


struct Buffer {
	Microsoft::WRL::ComPtr<ID3D12Resource> buffer{};
	D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_COMMON;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle{};

	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle{};
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle{};

	D3D12_CPU_DESCRIPTOR_HANDLE dpsCPUHandle{};

	uint32_t width;
	uint32_t height;

	CD3DX12_RESOURCE_BARRIER TransitionBarrier(D3D12_RESOURCE_STATES nextState) {
		CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			buffer.Get(),
			states,
			nextState);

		states = nextState;
		return barrier;
	}
};
 