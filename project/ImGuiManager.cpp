#include "ImGuiManager.h"

#include <cassert>

#include "SrvManager.h"

void ImGuiManager::Initialize([[maybe_unused]] Window* window, [[maybe_unused]] DirectXCommon* dxCommon) {
#ifdef USE_IMGUI
	assert(window);
	assert(dxCommon);

	dxCommon_ = dxCommon;
	SrvManager* srvManager = SrvManager::GetInstance();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window->GetHwnd());

	ImGui_ImplDX12_InitInfo initInfo = {};
	initInfo.Device = dxCommon->GetDevice();
	initInfo.CommandQueue = dxCommon->GetCommandQueue();
	initInfo.NumFramesInFlight = 2;
	initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	initInfo.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	initInfo.SrvDescriptorHeap = srvManager->GetDescriptorHeap();
	initInfo.UserData = srvManager;
	initInfo.SrvDescriptorAllocFn =
		[](ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle) {
			SrvManager* srvManager = static_cast<SrvManager*>(info->UserData);
			const uint32_t index = srvManager->Allocate();
			*outCpuHandle = srvManager->GetCPUDescriptorHandle(index);
			*outGpuHandle = srvManager->GetGPUDescriptorHandle(index);
		};
	initInfo.SrvDescriptorFreeFn =
		[](ImGui_ImplDX12_InitInfo*,
			D3D12_CPU_DESCRIPTOR_HANDLE,
			D3D12_GPU_DESCRIPTOR_HANDLE) {
		};

	const bool initialized = ImGui_ImplDX12_Init(&initInfo);
	assert(initialized);
#endif
}

void ImGuiManager::Finalize() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImGuiManager::Begin() {
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImGuiManager::End() {
#ifdef USE_IMGUI
	ImGui::Render();
#endif
}

void ImGuiManager::Draw() {
#ifdef USE_IMGUI
	assert(dxCommon_);

	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();
	ID3D12DescriptorHeap* descriptorHeaps[] = {
		SrvManager::GetInstance()->GetDescriptorHeap(),
	};

	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}
