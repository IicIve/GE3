#include "DirectXCommon.h"

#include <cassert>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

void DirectXCommon::Initialize() {
	HRESULT hr;

	//デバッグレイヤをオン
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

}
