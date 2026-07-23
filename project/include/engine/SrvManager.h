#pragma once

#include "DirectXCommon.h"

#include <wrl.h>

class SrvManager {
public:
	static SrvManager* GetInstance();

	void Initialize(DirectXCommon* dxCommon);
	void Finalize();
	void PreDraw();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap.Get(); }

	uint32_t Allocate();

private:
	static SrvManager* instance;

	void SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t srvIndex);
	void CreateSRVForTexture2D(uint32_t srvIndex, ID3D12Resource* resource, DXGI_FORMAT format, UINT mipLevels);
	void CreateSRVForStructuredBuffer(uint32_t srvIndex, ID3D12Resource* resource, UINT numElements, UINT structureByteStride);

	DirectXCommon* dxCommon = nullptr;

	static const uint32_t kMaxSrvCount;
	uint32_t descriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	uint32_t useIndex = 0;

	SrvManager() = default;
	~SrvManager() = default;
	SrvManager(SrvManager&) = delete;
	SrvManager& operator=(SrvManager&) = delete;
};
