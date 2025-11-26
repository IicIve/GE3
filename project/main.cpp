#include <Windows.h>
#include "cstdint"
#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <format>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include "dbghelp.h"
#include "strsafe.h"
#include <dxgidebug.h>
#include <dxcapi.h>
#include <Vector>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include <iostream>
#include <filesystem>
#include "externals/DirectXTex/d3dx12.h"
#include <numbers>
#include <wrl.h>
#include <xaudio2.h>
#include <direct.h>

#include "Input.h"
#include "Window.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "xaudio2.lib")


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);

typedef struct vector2 {
	float x;
	float y;
}Vector2;

typedef struct Vector3 {
	float x;
	float y;
	float z;
} Vector3;

typedef struct Vector4 {
	float x;
	float y;
	float z;
	float w;
} Vector4;

typedef struct Matrix3x3 {
	float m[3][3];
}Matrix3x3;

typedef struct Matrix4x4 {
	float m[4][4];
}Matrix4x4;

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct MaterialData {
	std::string textureFilePath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Sphere {
	Vector3 center;
	float radius;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight {
	Vector4 color;
	Vector3 direction;
	float intensity;
};

struct ChunkHeader {
	char id[4];
	int32_t size;
};

struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	unsigned int bufferSize;
};

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//CoInitializeEx(0, COINIT_MULTITHREADED);
	//SetUnhandledExceptionFilter(ExportDump);

	//ポインタ
	Input* input = nullptr;
	Window* window = nullptr;
	DirectXCommon* dxCommon = nullptr;

	//初期化
	window = new Window();
	dxCommon = new DirectXCommon();

	////関数の宣言
	//void Log(const std::string & message);
	//void Log(std::ostream & os, const std::string & message);
	//std::wstring ConvertString(const std::string & str);
	//std::string ConvertString(const std::wstring & str);

	//IDxcBlob* CompileShader(
	//	const std::wstring & filePath,
	//	const wchar_t* profile,
	//	IDxcUtils * dxcUtils,
	//	IDxcCompiler3 * dxcCompiler,
	//	IDxcIncludeHandler * includeHandler);

	//ID3D12Resource* CreateBufferResource(ID3D12Device * device, size_t sizeInBytes);

	Matrix4x4 MakeIdentity4x4();
	Matrix4x4 MakeAffineMatrix(const Vector3 & scale, const Vector3 & rotate, const Vector3 & translate);
	//Vector3 Transform(const Vector3 & vector, const Matrix4x4 & matrix);
	Matrix4x4 Inverse(const Matrix4x4 & m1);
	Matrix4x4 Multiply(const Matrix4x4 & m1, const Matrix4x4 & m2);
	Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
	Matrix4x4 MakePerspectiveFovMatrix(float fowY, float aspectRatio, float nearClip, float farClip);
	//Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);

	//ID3D12DescriptorHeap* CreateDescriptorHeap(
	//	ID3D12Device * device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);

	//DirectX::ScratchImage LoadTexture(const std::string & filePath);
	//ID3D12Resource* CreateTextureResource(ID3D12Device * device, const DirectX::TexMetadata & metadata);
	//ID3D12Resource* UploadTextureData(ID3D12Resource * texture, const DirectX::ScratchImage & mipImages,
	//	ID3D12Device * device, ID3D12GraphicsCommandList * commandList);
	////void UploadTextureData(ID3D12Resource * texture, const DirectX::ScratchImage & mipImages);

	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device * device, int32_t width, int32_t height);
	void CreateSphereVertices(int kSubdivision, float radius, std::vector<VertexData>&vertexData);
	//void DrawSphere(
	//	ID3D12Device * device,
	//	ID3D12GraphicsCommandList * commandList,
	//	const Sphere & sphere,
	//	Matrix4x4 wvpMatrix,
	//	ID3D12Resource * wvpResource,
	//	ID3D12Resource * materialResource,
	//	ID3D12DescriptorHeap * srvDescriptorHeap,
	//	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU,
	//	ID3D12RootSignature * rootSignature,
	//	ID3D12PipelineState * pipelineState,
	//	D3D12_VERTEX_BUFFER_VIEW vbView,
	//	std::vector<VertexData> vertexData
	//);

	//D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap * descriptorHeap, uint32_t descriptorSize, uint32_t index);
	//D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap * descriptorHeap, uint32_t descriptorSize, uint32_t index);

	ModelData LoadObjFile(const std::string & directoryPath, const std::string & filename);
	////MaterialData LoadMaterialTemplateFile(const std::string & directoryPath, const std::string & filename);

	////変数の宣言
	HRESULT hr;
	Transform transform{ {1.0f,1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f,}, {0.0f,0.0f, -5.0f} };
	Sphere sphere = { {0.0f, 0.0f, 0.0f}, 1.0f };
	Transform uvTransformSprite{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, };
	//std::vector<VertexData> sphereVertices;
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(Window::kClientWidth) / static_cast<float>(Window::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	//transformationMatrixData = worldViewProjectionMatrix;

	//スプライトの変換行列
	Matrix4x4 worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MakeOrthographicMatrix(0.0f, 0.0f, static_cast<float>(Window::kClientWidth), static_cast<float>(Window::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));

	//球の変換行列
	Matrix4x4 worldMatrixSphere = MakeAffineMatrix(
		{ sphere.radius, sphere.radius, sphere.radius }, // スケール
		{ 0.0f, 0.0f, 0.0f },                           // 回転
		sphere.center                                   // 平行移動
	);
	Matrix4x4 worldViewProjectionMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));

	//UVTransform用の行列
	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);

	//Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice* masterVoice;
	HRESULT result;

	window->Initialize();
	dxCommon->Initialize(window);

	//result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//result = xAudio2->CreateMasteringVoice(&masterVoice);

#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();

		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif

	////DXGIファクトリーの生成
	//IDXGIFactory7* dxgiFactory = nullptr;
	//HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//assert(SUCCEEDED(hr));

	////使用するアダプタ用の変数
	//IDXGIAdapter4* useAdapter = nullptr;
	////いい順にアダプタを頼む
	//for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(
	//	i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter))
	//	!= DXGI_ERROR_NOT_FOUND; ++i) {

	//	DXGI_ADAPTER_DESC3 adapterDesc{};
	//	hr = useAdapter->GetDesc3(&adapterDesc);
	//	assert(SUCCEEDED(hr));

	//	if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
	//		//Log(std::format("Use Adapter : {}\n", adapterDesc.Description));
	//		break;
	//	}
	//	useAdapter = nullptr;
	//}
	//assert(useAdapter != nullptr);

	//ID3D12Device* device = nullptr;
	//D3D_FEATURE_LEVEL featureLevels[] = {
	//	D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0
	//};
	//const char* featureLevelStrings[] = { "12.2", "12.1", "12.0" };

	//for (size_t i = 0; i < _countof(featureLevels); ++i) {
	//	hr = D3D12CreateDevice(useAdapter, featureLevels[i], IID_PPV_ARGS(&device));

	//	if (SUCCEEDED(hr)) {
	//		Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
	//		break;
	//	}
	//}

	//assert(device != nullptr);
	//Log("Complete create D3D12Device!\n");

#ifdef _DEBUG

	//エラーや警告を出す
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {

		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		D3D12_MESSAGE_ID denyIds[] = {
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};

		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		infoQueue->PushStorageFilter(&filter);

		infoQueue->Release();
	}

#endif

	////初期値0でFenceを作る
	//ID3D12Fence* fence = nullptr;
	//uint64_t fenceValue = 0;
	//hr = device->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	//assert(SUCCEEDED(hr));

	////fenceのsignalを持つためのイベントを作成する
	//HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	//assert(fenceEvent != nullptr);

	////dxCompilerを初期化
	//IDxcUtils* dxcUtils = nullptr;
	//IDxcCompiler3* dxcCompiler = nullptr;
	//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	//assert(SUCCEEDED(hr));
	//hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	//assert(SUCCEEDED(hr));

	//IDxcIncludeHandler* includeHandler = nullptr;
	//hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	//assert(SUCCEEDED(hr));

	////コマンドキューを生成する
	//ID3D12CommandQueue* commandQueue = nullptr;
	//D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	//assert(SUCCEEDED(hr));

	////コマンドアロケータを生成する
	//ID3D12CommandAllocator* commandAllocator = nullptr;
	//hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	//assert(SUCCEEDED(hr));

	//ID3D12GraphicsCommandList* commandList = nullptr;
	//hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	//assert(SUCCEEDED(hr));

	////スワップチェーンを生成する
	//IDXGISwapChain4* swapChain = nullptr;
	//DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	//swapChainDesc.Width = Window::kClientWidth;
	//swapChainDesc.Height = Window::kClientHeight;
	//swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//swapChainDesc.SampleDesc.Count = 1;
	//swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//swapChainDesc.BufferCount = 2;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	//hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, window->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	//assert(SUCCEEDED(hr));

	//const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	////ディスクリプタヒープの生成
	//ID3D12DescriptorHeap* rtvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	//ID3D12DescriptorHeap* srvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	////SwapChainからResourceを引っ張ってくる
	//ID3D12Resource* swapChainResources[2] = { nullptr };
	//hr = swapChain->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	//assert(SUCCEEDED(hr));
	//hr = swapChain->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	//assert(SUCCEEDED(hr));

	///*const uint32_t descriptorSizeSRV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//const uint32_t descriptorSizeRTV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//const uint32_t descriptorSizeDSV = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	//GetGPUDescriptorHandle(rtvDescriptorHeap, descriptorSizeRTV, 0);
	//GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, 0);*/

	////RTVの設定
	//D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{  };
	//rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStarHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	//rtvHandles[0] = rtvStarHandle;
	//device->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	//rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	//device->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);

	///*typedef struct D3D12_CPU_DESCRIPTOR_HANDLE {
	//	SIZE_T ptr;
	//}D3D12_CPU_DESCRIPTOR_HANDLE;*/

	//rtvHandles[0] = rtvStarHandle;
	//rtvHandles[1].ptr = rtvHandles[0].ptr + device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	////コマンドを積む
	//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

	////TransitionBarrierを張るコード
	//D3D12_RESOURCE_BARRIER barrier{};
	//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier.Transition.pResource = swapChainResources[backBufferIndex];
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//commandList->ResourceBarrier(1, &barrier);

	//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, nullptr);
	//float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	//commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);

	////画面表示できるように状態を遷移
	//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	//commandList->ResourceBarrier(1, &barrier);

	//hr = commandList->Close();
	//assert(SUCCEEDED(hr));

	////GPUにコマンドリストの実行を行わせる
	//ID3D12CommandList* commandLists[] = { commandList };
	//commandQueue->ExecuteCommandLists(1, commandLists);
	//swapChain->Present(1, 0);

	////Fenceの値を更新
	//fenceValue++;
	//commandQueue->Signal(fence, fenceValue);

	////Fenceの値が指定したSignal値にたどり着いているか確認する
	//if (fence->GetCompletedValue() < fenceValue) {

	//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
	//	WaitForSingleObject(fenceEvent, INFINITE);

	//}

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[1].Descriptor.ShaderRegister = 0;
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[3].Descriptor.ShaderRegister = 1;
	descriptionRootSignature.pParameters = rootParameters;
	descriptionRootSignature.NumParameters = _countof(rootParameters);

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	/*D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;*/

	float triangleColor[3] = { 1.0f, 1.0f, 1.0f };

	//マテリアル用のリソース作成
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = dxCommon->CreateBufferResource(256);
	Material* materialData = nullptr;
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(triangleColor[0], triangleColor[1], triangleColor[2], 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();
	/*ID3D12Resource* materialResource = CreateBufferResource(device, sizeof(Vector4));
	Vector4* materialData = nullptr;
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	*materialData = Vector4(triangleColor[0], triangleColor[1], triangleColor[2], 1.0f);*/

	//スプライトのマテリアル用のリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResourceSprite = dxCommon->CreateBufferResource(256);
	Material* materialDataSprite = nullptr;
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = Vector4(triangleColor[0], triangleColor[1], triangleColor[2], 1.0f);
	materialDataSprite->enableLighting = false;
	materialDataSprite->uvTransform = MakeIdentity4x4();
	materialDataSprite->uvTransform = uvTransformMatrix;
	/*ID3D12Resource* materialResourceSprite = CreateBufferResource(device, sizeof(Material));
	Material* materialDataSprite = nullptr;
	materialResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = Vector4(triangleColor[0], triangleColor[1], triangleColor[2], 1.0f);
	materialDataSprite->enableLighting = false;*/

	//WVP用のリソース作成
	constexpr size_t kCBSize = (sizeof(TransformationMatrix) + 255) & ~255;
	Microsoft::WRL::ComPtr <ID3D12Resource> wvpResource = dxCommon->CreateBufferResource(kCBSize);
	//ID3D12Resource* wvpResource = CreateBufferResource(device, sizeof(TransformationMatrix));
	TransformationMatrix* wvpData = nullptr;
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	wvpData->WVP = worldViewProjectionMatrix;
	wvpData->World = worldMatrix;
	*wvpData = { wvpData->WVP,wvpData->World };

	//光源用のリソース作成
	Microsoft::WRL::ComPtr <ID3D12Resource> directionalLightResource = dxCommon->CreateBufferResource(sizeof(DirectionalLight));
	DirectionalLight* directionalLightData = nullptr;
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));

	// 値をセット
	directionalLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f }; // 白色
	directionalLightData->direction = { -1.0f, -1.0f, 0.0f };  // 下向き
	directionalLightData->intensity = 1.0f;

	//Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResourceSprite = dxCommon->CreateBufferResource(sizeof(VertexData) * 6);
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite->GetGPUVirtualAddress();
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	//sprite用のTransformationMatrix用のリソースを作る
	constexpr size_t kCBSizeSprite = (sizeof(Matrix4x4) + 255) & ~255;
	Microsoft::WRL::ComPtr <ID3D12Resource> transformationMatrixResourceSprite = dxCommon->CreateBufferResource(kCBSizeSprite);
	//ID3D12Resource* transformationMatrixResourceSprite = CreateBufferResource(device, sizeof(Matrix4x4));
	Matrix4x4* transformationMatrixDataSprite = nullptr;
	transformationMatrixResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	*transformationMatrixDataSprite = MakeIdentity4x4();

	//spriteのindexResoureを作る
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResourceSprite = dxCommon->CreateBufferResource(sizeof(uint32_t) * 6);
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	indexBufferViewSprite.BufferLocation = indexResourceSprite->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;

	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Logger::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.VS.hlsl",L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3d.PS.hlsl",L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

	//depthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	//PSOを生成
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipeLineStateDesc{};
	graphicsPipeLineStateDesc.pRootSignature = rootSignature;
	graphicsPipeLineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipeLineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize() };
	graphicsPipeLineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize() };
	graphicsPipeLineStateDesc.BlendState = blendDesc;
	graphicsPipeLineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipeLineStateDesc.NumRenderTargets = 1;
	graphicsPipeLineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipeLineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	graphicsPipeLineStateDesc.SampleDesc.Count = 1;
	graphicsPipeLineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//depthStencilの設定
	graphicsPipeLineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipeLineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(&graphicsPipeLineStateDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

	//頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;

	D3D12_RESOURCE_DESC vertexResourceDesc{  };
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(Vector4) * 3;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//depthStencilView
	/*ID3D12Resource* vertexResource = nullptr;
	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));*/

	//ID3D12Resource* vertexResource = CreateBufferResource(device, sizeof(VertexData) * 6);

	////頂点バッファビューを作成
	///*D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//vertexBufferView.SizeInBytes = sizeof(VertexData) * 6;
	//vertexBufferView.StrideInBytes = sizeof(VertexData);*/

	//モデル読み込み
	ModelData modelData = LoadObjFile("resources", "axis.obj");
	//モデルも頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());
	//頂点バッファビューを作る
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	//頂点リソースにデータを書き込む
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	////SRV
	////頂点リソースにデータを書き込む
	////VertexData* vertexData = nullptr;
	////vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	////頂点データの設定
	///*vertexData[0].position = { -0.5f,-0.5f,0.0f, 1.0f };
	//vertexData[0].texcoord = { 0.0f, 1.0f };
	//vertexData[1].position = { 0.0f, 0.5f, 0.0f, 1.0f };
	//vertexData[1].texcoord = { 0.5f, 0.0f };
	//vertexData[2].position = { 0.5f, -0.5f, 0.0f, 1.0f };
	//vertexData[2].texcoord = { 1.0f, 1.0f };

	//vertexData[3].position = { -0.5f, -0.5f, 0.5f, 1.0f };
	//vertexData[3].texcoord = { 0.0f, 1.0f };
	//vertexData[4].position = { 0.0f, 0.0f,0.0f,1.0f };
	//vertexData[4].texcoord = { 0.5f, 0.0f };
	//vertexData[5].position = { 0.5f, -0.5f,-0.5f,1.0f };
	//vertexData[5].texcoord = { 1.0f, 1.0f };
	//for (int index = 0; index < 6; ++index) {
	//	vertexData[index].normal.x = vertexData[index].position.x;
	//	vertexData[index].normal.y = vertexData[index].position.y;
	//	vertexData[index].normal.z = vertexData[index].position.z;
	//}*/

	//頂点リソースにデータを書き込む
	VertexData* vertexDataSprite = nullptr;
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//スプライトの頂点データの設定
	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f, 1.0f };
	vertexDataSprite[0].texcoord = { 0.0f, 1.0f };
	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[1].texcoord = { 0.0f, 0.0f };
	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[2].texcoord = { 1.0f, 1.0f };

	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texcoord = { 1.0f, 0.0f };
	/*vertexDataSprite[4].position = { 640.0f, 0.0f,0.0f,1.0f };
	vertexDataSprite[4].texcoord = { 1.0f, 0.0f };
	vertexDataSprite[5].position = { 640.0f, 360.0f,0.0f,1.0f };
	vertexDataSprite[5].texcoord = { 1.0f, 1.0f };*/

	vertexDataSprite[0].normal = { 0.0f,0.0f,-1.0f };

	//indexResourceにデータを書き込む
	uint32_t* indexDataSprite = nullptr;
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0;
	indexDataSprite[1] = 1;
	indexDataSprite[2] = 2;
	indexDataSprite[3] = 1;
	indexDataSprite[4] = 3;
	indexDataSprite[5] = 2;

	// 球の頂点データ生成
	std::vector<VertexData> vertexDataSphere;
	const int kSubdivision = 32;
	CreateSphereVertices(kSubdivision, 1.0f, vertexDataSphere);

	// 頂点バッファ作成
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBufferSphere = dxCommon->CreateBufferResource(sizeof(VertexData) * vertexDataSphere.size());
	void* mapped = nullptr;
	vertexBufferSphere->Map(0, nullptr, &mapped);
	memcpy(mapped, vertexDataSphere.data(), sizeof(VertexData) * vertexDataSphere.size());
	vertexBufferSphere->Unmap(0, nullptr);

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	vbView.BufferLocation = vertexBufferSphere->GetGPUVirtualAddress();
	vbView.SizeInBytes = UINT(sizeof(VertexData) * vertexDataSphere.size());
	vbView.StrideInBytes = sizeof(VertexData);

	////ビューポート
	//D3D12_VIEWPORT viewport{};
	//viewport.Width = Window::kClientWidth;
	//viewport.Height = Window::kClientHeight;
	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;

	////シザー矩形
	//D3D12_RECT scissorRect{};
	//scissorRect.left = 0;
	//scissorRect.right = Window::kClientWidth;
	//scissorRect.top = 0;
	//scissorRect.bottom = Window::kClientHeight;

	////ImGuiの初期化
	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//ImGui_ImplWin32_Init(window->GetHwnd());
	//ImGui_ImplDX12_Init(device, swapChainDesc.BufferCount, rtvDesc.Format, srvDescriptorHeap,
	//	srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());

	//Textureを読んで転送する
	char currentDir[256];
	if (_getcwd(currentDir, sizeof(currentDir))) {
		std::cout << "Current directory: " << currentDir << std::endl;
	} else {
		std::cerr << "Failed to get current directory." << std::endl;
	}

	DirectX::ScratchImage mipImages = dxCommon->LoadTexture("resources/uvChecker.png");
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

	////2枚目のtexture
	DirectX::ScratchImage mipImages2 = dxCommon->LoadTexture(modelData.material.textureFilePath);
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();

	/*if (fence->GetCompletedValue() < fenceValue) {
		fence->SetEventOnCompletion(fenceValue, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));*/

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = dxCommon->CreateTextureResource(metadata);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = dxCommon->UploadTextureData(textureResource, mipImages);

	//2枚目
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource2 = dxCommon->CreateTextureResource(metadata2);
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource2 = dxCommon->UploadTextureData(textureResource2, mipImages2);

	//hr = commandList->Close();
	////GPUにコマンドリストの実行を行わせる
	//commandQueue->ExecuteCommandLists(1, commandLists);
	//swapChain->Present(1, 0);

	////Fenceの値を更新
	//fenceValue++;
	//hr = commandQueue->Signal(fence, fenceValue);
	//assert(SUCCEEDED(hr));

	//metaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	//2枚目のtextureのmetaDataを基にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	//SRVを作成するDescriptorHeapの場所を決める
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = dxCommon->GetSRVCPUDescriptorHandle(1);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = dxCommon->GetSRVGPUDescriptorHandle(1);
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	/*D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(textureResource, &srvDesc, textureSrvHandleCPU);*/

	//2枚目のtextureのSRV生成
	/*D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	textureSrvHandleCPU2.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU2.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(textureResource2, &srvDesc2, textureSrvHandleCPU2);*/
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = dxCommon->GetSRVCPUDescriptorHandle(2);
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = dxCommon->GetSRVGPUDescriptorHandle(2);
	dxCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);


	//DepthStencilTextureをウィンドウサイズで作成
	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(dxCommon->GetDevice(), Window::kClientWidth, Window::kClientHeight);

	////DSV用のヒープでデイスクリプタの数は1
	//ID3D12DescriptorHeap* dsvDescriptorHeap = CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);
	////DSVの設定
	//D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	//dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	//device->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	////描画先のRTVとDSVを設定する
	//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	//SRV切り替え用bool変数
	bool useMonsterBall = false;

	//入力の初期化
	input = new Input();
	input->Initialize(window);

	//メインループ
	MSG msg{};
	//ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {

		//Windowsのメッセージ処理
		if (window->ProcessMessage()) {
			break;
		}

		//ゲームの処理
		//入力の更新
		input->Update();


		cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
		viewMatrix = Inverse(cameraMatrix);

		transform.rotate.y += 0.01f;
		worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
		worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		wvpData->WVP = worldViewProjectionMatrix;
		wvpData->World = worldMatrix;
		//*wvpData = worldViewProjectionMatrix;

		worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
		*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;

		uvTransformMatrix = MakeAffineMatrix(uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);
		materialDataSprite->uvTransform = uvTransformMatrix;

		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n");
		}

		//ImGui
		//ImGui_ImplDX12_NewFrame();
		//ImGui_ImplWin32_NewFrame();
		//ImGui::NewFrame();
		//ImGui::ShowDemoWindow();
		//ImGui::DragFloat3("cameraScale", &cameraTransform.scale.x, 0.01f);
		//ImGui::DragFloat3("cameraRotate", &cameraTransform.rotate.x, 0.01f);
		//ImGui::DragFloat3("cameraTranslate", &cameraTransform.translate.x, 0.01f);
		//ImGui::DragFloat3("transform", &transform.translate.x, 0.01f);
		//ImGui::DragFloat2("transformSprite", &transformSprite.translate.x, 1.0f);
		//ImGui::DragFloat3("Light", &directionalLightData->direction.x, 0.01f);
		////ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 1.0f);
		//ImGui::ColorEdit3("Triangle Color", triangleColor);
		////ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//*materialData = Material{ Vector4{triangleColor[0], triangleColor[1], triangleColor[2], 0.0f}, 1 };
		//*materialData = Material{ Vector4{triangleColor[0], triangleColor[1], triangleColor[2], 1.0f}, 1, {0,0,0}, MakeIdentity4x4() };
		////*materialData = Vector4(triangleColor[0], triangleColor[1], triangleColor[2], 1.0f);

		{
			Vector3& dir = directionalLightData->direction;
			float len = std::sqrt(dir.x * dir.x + dir.y * dir.y + dir.z * dir.z);
			if (len > 0.0001f) {
				dir.x /= len;
				dir.y /= len;
				dir.z /= len;
			}
		}

		//if (fence->GetCompletedValue() < fenceValue) {
		//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
		//	WaitForSingleObject(fenceEvent, INFINITE);
		//}

		//hr = commandAllocator->Reset();
		//assert(SUCCEEDED(hr));
		//hr = commandList->Reset(commandAllocator, nullptr);
		//assert(SUCCEEDED(hr));

		//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();

		////TransitionBarrierを張るコード
		//D3D12_RESOURCE_BARRIER barrier{};
		//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//barrier.Transition.pResource = swapChainResources[backBufferIndex];
		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//commandList->ResourceBarrier(1, &barrier);

		//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
		//float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
		//commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
		//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
		//dxCommon->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);

		//commandList->RSSetViewports(1, &viewport);
		//commandList->RSSetScissorRects(1, &scissorRect);

		dxCommon->PreDraw();

		dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature);
		dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState);
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);

		dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
		dxCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

		////DrawSphere(
		////	device,                        // ID3D12Device*
		////	commandList,                   // ID3D12GraphicsCommandList*
		////	sphere,                        // 描画したい球
		////	worldViewProjectionMatrixSphere,
		////	wvpResource,                   // WVP用の定数バッファリソース
		////	materialResource,              // マテリアル用の定数バッファリソース
		////	srvDescriptorHeap,             // SRVディスクリプタヒープ
		////	useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU,           // テクスチャSRVのGPUハンドル
		////	rootSignature,                 // ルートシグネチャ
		////	graphicsPipelineState,
		////	vbView,
		////	vertexDataSphere
		////);

		dxCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
		//Spriteの描画
		dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
		dxCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite->GetGPUVirtualAddress());
		dxCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

		/*ImGui::Render();
		ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), dxCommon->GetCommandList());*/

		//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		//commandList->ResourceBarrier(1, &barrier);

		//hr = commandList->Close();

		////GPUにコマンドリストの実行を行わせる
		//ID3D12CommandList* commandLists[] = { commandList };
		//commandQueue->ExecuteCommandLists(1, commandLists);
		//swapChain->Present(1, 0);

		////Fenceの値を更新
		//fenceValue++;
		//hr = commandQueue->Signal(fence, fenceValue);
		//assert(SUCCEEDED(hr));

		dxCommon->PostDraw();

	}

	//解放処理
	/*ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();*/
	/*CloseHandle(fenceEvent);
	fence->Release();
	rtvDescriptorHeap->Release();
	srvDescriptorHeap->Release();
	swapChainResources[0]->Release();
	swapChainResources[1]->Release();
	swapChain->Release();
	commandList->Release();
	commandAllocator->Release();
	commandQueue->Release();
	device->Release();
	useAdapter->Release();
	dxgiFactory->Release();*/

	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();
	materialResource->Release();
	materialResourceSprite->Release();
	wvpResource->Release();
	textureResource->Release();
	textureResource2->Release();
	depthStencilResource->Release();
	//dsvDescriptorHeap->Release();
	vertexResourceSprite->Release();
	transformationMatrixResourceSprite->Release();
	intermediateResource->Release();
	intermediateResource2->Release();
	vertexBufferSphere->Release();
	directionalLightResource->Release();
	indexResourceSprite->Release();
#ifdef _DEBUG

	debugController->Release();

#endif

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	window->Finalize();

	delete input;
	delete window;
	delete dxCommon;

	return 0;
}

//void Log(const std::string& message) {
//	OutputDebugStringA(message.c_str());
//}
//
//void Log(std::ostream& os, const std::string& message) {
//	os << message << std::endl;
//	OutputDebugStringA(message.c_str());
//}
//
//std::wstring ConvertString(const std::string& str) {
//	if (str.empty()) {
//		return std::wstring();
//	}
//
//	auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
//	if (sizeNeeded == 0) {
//		return std::wstring();
//	}
//	std::wstring result(sizeNeeded, 0);
//	MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
//	return result;
//}
//
//std::string ConvertString(const std::wstring& str) {
//	if (str.empty()) {
//		return std::string();
//	}
//
//	auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
//	if (sizeNeeded == 0) {
//		return std::string();
//	}
//	std::string result(sizeNeeded, 0);
//	WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
//	return result;
//}
//
static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception) {
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);

	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();

	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;

	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);

	return EXCEPTION_EXECUTE_HANDLER;
}

//IDxcBlob* CompileShader(
//	const std::wstring& filePath,
//	const wchar_t* profile,
//	IDxcUtils* dxcUtils,
//	IDxcCompiler3* dxcCompiler,
//	IDxcIncludeHandler* includeHandler) {
//
//	//hlslファイルを読む
//	Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
//	IDxcBlobEncoding* shaderSource = nullptr;
//	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
//	assert(SUCCEEDED(hr));
//
//	DxcBuffer shaderSourceBuffer;
//	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
//	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
//	shaderSourceBuffer.Encoding = DXC_CP_UTF8;
//
//	LPCWSTR arguments[] = {
//		filePath.c_str(),
//		L"-E", L"main",
//		L"-T", profile,
//		L"-Zi", L"-Qembed_debug",
//		L"-Od",
//		L"-Zpr",
//	};
//
//	//実際にShaderをコンパイルする
//	IDxcResult* shaderResult = nullptr;
//	hr = dxcCompiler->Compile(
//		&shaderSourceBuffer,
//		arguments,
//		_countof(arguments),
//		includeHandler,
//		IID_PPV_ARGS(&shaderResult)
//	);
//	assert(SUCCEEDED(hr));
//
//	//エラーが出たらログに出して止める
//	IDxcBlobUtf8* shaderError = nullptr;
//	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
//	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
//		Log(shaderError->GetStringPointer());
//		assert(false);
//	}
//
//	//コンパイル結果から実行用のバイナリ部分を取得
//	IDxcBlob* shaderBlob = nullptr;
//	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
//	assert(SUCCEEDED(hr));
//	Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
//	shaderSource->Release();
//	shaderResult->Release();
//
//	return shaderBlob;
//}
//
//ID3D12Resource* CreateBufferResource(ID3D12Device* device, size_t sizeInBytes) {
//	HRESULT hr;
//	//ID3D12Resource* result;
//
//	//頂点リソース用のヒープの設定
//	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
//	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
//
//	D3D12_RESOURCE_DESC vertexResourceDesc{  };
//	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
//	vertexResourceDesc.Width = sizeInBytes;
//
//	vertexResourceDesc.Height = 1;
//	vertexResourceDesc.DepthOrArraySize = 1;
//	vertexResourceDesc.MipLevels = 1;
//	vertexResourceDesc.SampleDesc.Count = 1;
//
//	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
//
//	ID3D12Resource* result = nullptr;
//	hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
//		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&result));
//	assert(SUCCEEDED(hr));
//
//	return result;
//}
//
Matrix4x4 MakeIdentity4x4() {
	Matrix4x4 result = {};

	for (int i = 0; i < 4; ++i) {
		result.m[i][i] = 1.0f;
	}

	return result;
}

Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result;

	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			result.m[row][col] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				result.m[row][col] += m1.m[row][k] * m2.m[k][col];
			}
		}
	}

	return result;
}

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {

	Matrix4x4 MakeRotateXMatrix;

	MakeRotateXMatrix.m[0][0] = 1.0f;
	MakeRotateXMatrix.m[0][1] = 0.0f;
	MakeRotateXMatrix.m[0][2] = 0.0f;
	MakeRotateXMatrix.m[0][3] = 0.0f;

	MakeRotateXMatrix.m[1][0] = 0.0f;
	MakeRotateXMatrix.m[1][1] = std::cos(rotate.x);
	MakeRotateXMatrix.m[1][2] = std::sin(rotate.x);
	MakeRotateXMatrix.m[1][3] = 0.0f;

	MakeRotateXMatrix.m[2][0] = 0.0f;
	MakeRotateXMatrix.m[2][1] = -std::sin(rotate.x);
	MakeRotateXMatrix.m[2][2] = std::cos(rotate.x);
	MakeRotateXMatrix.m[2][3] = 0.0f;

	MakeRotateXMatrix.m[3][0] = 0.0f;
	MakeRotateXMatrix.m[3][1] = 0.0f;
	MakeRotateXMatrix.m[3][2] = 0.0f;
	MakeRotateXMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateYMatrix;

	MakeRotateYMatrix.m[0][0] = std::cos(rotate.y);
	MakeRotateYMatrix.m[0][1] = 0.0f;
	MakeRotateYMatrix.m[0][2] = -std::sin(rotate.y);
	MakeRotateYMatrix.m[0][3] = 0.0f;

	MakeRotateYMatrix.m[1][0] = 0.0f;
	MakeRotateYMatrix.m[1][1] = 1.0f;
	MakeRotateYMatrix.m[1][2] = 0.0f;
	MakeRotateYMatrix.m[1][3] = 0.0f;

	MakeRotateYMatrix.m[2][0] = std::sin(rotate.y);
	MakeRotateYMatrix.m[2][1] = 0.0f;
	MakeRotateYMatrix.m[2][2] = std::cos(rotate.y);
	MakeRotateYMatrix.m[2][3] = 0.0f;

	MakeRotateYMatrix.m[3][0] = 0.0f;
	MakeRotateYMatrix.m[3][1] = 0.0f;
	MakeRotateYMatrix.m[3][2] = 0.0f;
	MakeRotateYMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateZMatrix;

	MakeRotateZMatrix.m[0][0] = std::cos(rotate.z);
	MakeRotateZMatrix.m[0][1] = std::sin(rotate.z);
	MakeRotateZMatrix.m[0][2] = 0.0f;
	MakeRotateZMatrix.m[0][3] = 0.0f;

	MakeRotateZMatrix.m[1][0] = -std::sin(rotate.z);
	MakeRotateZMatrix.m[1][1] = std::cos(rotate.z);
	MakeRotateZMatrix.m[1][2] = 0.0f;
	MakeRotateZMatrix.m[1][3] = 0.0f;

	MakeRotateZMatrix.m[2][0] = 0.0f;
	MakeRotateZMatrix.m[2][1] = 0.0f;
	MakeRotateZMatrix.m[2][2] = 1.0f;
	MakeRotateZMatrix.m[2][3] = 0.0f;

	MakeRotateZMatrix.m[3][0] = 0.0f;
	MakeRotateZMatrix.m[3][1] = 0.0f;
	MakeRotateZMatrix.m[3][2] = 0.0f;
	MakeRotateZMatrix.m[3][3] = 1.0f;

	Matrix4x4 MakeRotateXYZMatrix = Multiply(MakeRotateXMatrix, Multiply(MakeRotateYMatrix, MakeRotateZMatrix));

	Matrix4x4 result;

	result.m[0][0] = MakeRotateXYZMatrix.m[0][0] * scale.x;
	result.m[0][1] = MakeRotateXYZMatrix.m[0][1] * scale.x;
	result.m[0][2] = MakeRotateXYZMatrix.m[0][2] * scale.x;
	result.m[0][3] = 0.0f;

	result.m[1][0] = MakeRotateXYZMatrix.m[1][0] * scale.y;
	result.m[1][1] = MakeRotateXYZMatrix.m[1][1] * scale.y;
	result.m[1][2] = MakeRotateXYZMatrix.m[1][2] * scale.y;
	result.m[1][3] = 0.0f;

	result.m[2][0] = MakeRotateXYZMatrix.m[2][0] * scale.z;
	result.m[2][1] = MakeRotateXYZMatrix.m[2][1] * scale.z;
	result.m[2][2] = MakeRotateXYZMatrix.m[2][2] * scale.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

////Vector3 Transform(const Vector3& vector, const Matrix4x4& matrix) {
////	Vector3 result;
////
////	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
////	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
////	result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];
////	float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
////
////
////	assert(w != 0.0f);
////	result.x /= w;
////	result.y /= w;
////	result.z /= w;
////
////	return result;
////}
//
Matrix4x4 Inverse(const Matrix4x4& m1) {
	Matrix4x4 result = {};
	float det = 0.0f;

	// 行列式を計算
	det = m1.m[0][0] * (m1.m[1][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[1][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1])) -
		m1.m[0][1] * (m1.m[1][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
			m1.m[1][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
			m1.m[1][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0])) +
		m1.m[0][2] * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) -
			m1.m[1][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
			m1.m[1][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0])) -
		m1.m[0][3] * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
			m1.m[1][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
			m1.m[1][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));

	// 逆行列を計算
	float invDet = 1.0f / det;

	result.m[0][0] = invDet * (m1.m[1][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[1][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]));
	result.m[0][1] = -invDet * (m1.m[0][1] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][1]) +
		m1.m[0][3] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]));
	result.m[0][2] = invDet * (m1.m[0][1] * (m1.m[1][2] * m1.m[3][3] - m1.m[1][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[1][1] * m1.m[3][3] - m1.m[1][3] * m1.m[3][1]) +
		m1.m[0][3] * (m1.m[1][1] * m1.m[3][2] - m1.m[1][2] * m1.m[3][1]));
	result.m[0][3] = -invDet * (m1.m[0][1] * (m1.m[1][2] * m1.m[2][3] - m1.m[1][3] * m1.m[2][2]) -
		m1.m[0][2] * (m1.m[1][1] * m1.m[2][3] - m1.m[1][3] * m1.m[2][1]) +
		m1.m[0][3] * (m1.m[1][1] * m1.m[2][2] - m1.m[1][2] * m1.m[2][1]));

	result.m[1][0] = -invDet * (m1.m[1][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[1][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]));
	result.m[1][1] = invDet * (m1.m[0][0] * (m1.m[2][2] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]));
	result.m[1][2] = -invDet * (m1.m[0][0] * (m1.m[1][2] * m1.m[3][3] - m1.m[1][3] * m1.m[3][2]) -
		m1.m[0][2] * (m1.m[1][0] * m1.m[3][3] - m1.m[1][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[3][2] - m1.m[1][2] * m1.m[3][0]));
	result.m[1][3] = invDet * (m1.m[0][0] * (m1.m[1][2] * m1.m[2][3] - m1.m[1][3] * m1.m[2][2]) -
		m1.m[0][2] * (m1.m[1][0] * m1.m[2][3] - m1.m[1][3] * m1.m[2][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[2][2] - m1.m[1][2] * m1.m[2][0]));

	result.m[2][0] = invDet * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[1][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[1][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[2][1] = -invDet * (m1.m[0][0] * (m1.m[2][1] * m1.m[3][3] - m1.m[2][3] * m1.m[3][2]) -
		m1.m[0][1] * (m1.m[2][0] * m1.m[3][3] - m1.m[2][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[2][2] = invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[3][3] - m1.m[1][3] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[3][3] - m1.m[1][3] * m1.m[3][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[3][1] - m1.m[1][1] * m1.m[3][0]));
	result.m[2][3] = -invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[2][3] - m1.m[1][3] * m1.m[2][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[2][3] - m1.m[1][3] * m1.m[2][0]) +
		m1.m[0][3] * (m1.m[1][0] * m1.m[2][1] - m1.m[1][1] * m1.m[2][0]));

	result.m[3][0] = -invDet * (m1.m[1][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
		m1.m[1][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
		m1.m[1][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[3][1] = invDet * (m1.m[0][0] * (m1.m[2][1] * m1.m[3][2] - m1.m[2][2] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[2][0] * m1.m[3][2] - m1.m[2][2] * m1.m[3][0]) +
		m1.m[0][2] * (m1.m[2][0] * m1.m[3][1] - m1.m[2][1] * m1.m[3][0]));
	result.m[3][2] = -invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[3][2] - m1.m[1][2] * m1.m[3][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[3][2] - m1.m[1][2] * m1.m[3][0]) +
		m1.m[0][2] * (m1.m[1][0] * m1.m[3][1] - m1.m[1][1] * m1.m[3][0]));
	result.m[3][3] = invDet * (m1.m[0][0] * (m1.m[1][1] * m1.m[2][2] - m1.m[1][2] * m1.m[2][1]) -
		m1.m[0][1] * (m1.m[1][0] * m1.m[2][2] - m1.m[1][2] * m1.m[2][0]) +
		m1.m[0][2] * (m1.m[1][0] * m1.m[2][1] - m1.m[1][1] * m1.m[2][0]));

	return result;
}

Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result;

	result.m[0][0] = 2.0f / (right - left);
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[2][3] = 0.0f;

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1.0f;

	return result;
}

Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};

	float yScale = 1.0f / std::tan(fovY / 2.0f);
	float xScale = yScale / aspectRatio;

	result.m[0][0] = xScale;
	result.m[1][1] = yScale;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);

	return result;
}

Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result;

	result.m[0][0] = width / 2.0f;
	result.m[0][1] = 0.0f;
	result.m[0][2] = 0.0f;
	result.m[0][3] = 0.0f;

	result.m[1][0] = 0.0f;
	result.m[1][1] = -(height / 2.0f);
	result.m[1][2] = 0.0f;
	result.m[1][3] = 0.0f;

	result.m[2][0] = 0.0f;
	result.m[2][1] = 0.0f;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0.0f;

	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = minDepth;
	result.m[3][3] = 1.0f;

	return result;
}

//ID3D12DescriptorHeap* CreateDescriptorHeap(
//	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible) {
//
//	//ディスクリプタヒープの生成
//	ID3D12DescriptorHeap* descriptorHeap = nullptr;
//	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
//	descriptorHeapDesc.Type = heapType;
//	descriptorHeapDesc.NumDescriptors = numDescriptors;
//	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
//	assert(SUCCEEDED(hr));
//
//	return descriptorHeap;
//}
//
//DirectX::ScratchImage LoadTexture(const std::string& filePath) {
//	//テクスチャファイルを読んでプログラムで扱えるようにする
//	DirectX::ScratchImage image{};
//	std::wstring filePathW = ConvertString(filePath);
//
//	// デバッグ表示
//	std::wcout << L"Loading texture: " << filePathW << std::endl;
//
//	// ファイル存在確認
//	std::filesystem::path texturePath = std::filesystem::absolute(filePath);
//	std::wcout << L"Absolute path: " << texturePath.wstring() << std::endl;
//	if (!std::filesystem::exists(texturePath)) {
//		std::cerr << "File not found: " << texturePath << std::endl;
//		assert(false);
//	}
//	HRESULT hr = DirectX::LoadFromWICFile(texturePath.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//
//	//if (!std::filesystem::exists(filePath)) {
//	//	std::cerr << "File not found: " << filePath << std::endl;
//	//	assert(false); // または return {};
//	//}
//	//HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
//
//	if (FAILED(hr)) {
//		std::wcout << L"Failed to load texture: " << filePathW << L", HRESULT = " << std::hex << hr << std::endl;
//	}
//
//	//assert(SUCCEEDED(hr));
//
//	//ミップマップの作成
//	DirectX::ScratchImage mipImages{};
//	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
//	assert(SUCCEEDED(hr));
//
//	return mipImages;
//}
//
//ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
//	//metadataを基にResourceの設定
//	D3D12_RESOURCE_DESC resourceDesc{};
//	resourceDesc.Width = UINT(metadata.width);
//	resourceDesc.Height = UINT(metadata.height);
//	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
//	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
//	resourceDesc.Format = metadata.format;
//	resourceDesc.SampleDesc.Count = 1;
//	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
//
//	//利用するHeapの設定
//	D3D12_HEAP_PROPERTIES heapProperties{};
//	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
//	//heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
//	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
//
//	//Resourceの生成
//	ID3D12Resource* resource = nullptr;
//	HRESULT hr = device->CreateCommittedResource(
//		&heapProperties,
//		D3D12_HEAP_FLAG_NONE,
//		&resourceDesc,
//		D3D12_RESOURCE_STATE_COPY_DEST,
//		nullptr,
//		IID_PPV_ARGS(&resource));
//	assert(SUCCEEDED(hr));
//
//	return resource;
//}
//
//[[nodiscard]]
//ID3D12Resource* UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages,
//	ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
//	//中間リソースの作成
//	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
//	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
//	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
//	ID3D12Resource* intermediateResource = CreateBufferResource(device, intermediateSize);
//	UpdateSubresources(commandList, texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());
//	D3D12_RESOURCE_BARRIER barrier{};
//	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//	barrier.Transition.pResource = texture;
//	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
//	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
//	commandList->ResourceBarrier(1, &barrier);
//
//	return intermediateResource;
//
//	////Meta情報を取得
//	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
//
//	////	全MipMapについて
//	//for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
//
//	//	const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
//	//	HRESULT hr = texture->WriteToSubresource(
//	//		UINT(mipLevel),
//	//		nullptr,
//	//		img->pixels,
//	//		UINT(img->rowPitch),
//	//		UINT(img->slicePitch)
//	//	);
//	//	assert(SUCCEEDED(hr));
//	//}
//}
//
ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	//生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.MipLevels = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthClearValue,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));

	return resource;
}

D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}

ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			//面は三角形限定
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (uint32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				//頂点を構築
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position, texcoord, normal };
				//VertexData vertex = { position, texcoord, normal };
				//modelData.vertices.push_back(vertex);
			}
			modelData.vertices.push_back(triangle[0]);
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

////SoundData SoundLoadWave(const char* filename) {
////	HRESULT result;
////
////	//ファイルを開く
////	std::ifstream file;
////	file.open(filename, std::ios_base::binary);
////	assert(file.is_open());
////
////	//RIFFヘッダーの読み込み
////	RiffHeader riff;
////	file.read((char*)&riff, sizeof(riff));
////	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
////		assert(0);
////	}
////	if (strncmp(riff.type, "WAVE", 4) != 0) {
////		assert(0);
////	}
////
////	//Formatチャンクの読み込み
////	FormatChunk format = {};
////	file.read((char*)&format, sizeof(ChunkHeader));
////	if (strncmp(format.chunk.id, "fmt", 4)) {
////		assert(0);
////	}
////	assert(format.chunk.size <= sizeof(ChunkHeader));
////	file.read((char*)&format.fmt, format.chunk.size);
////
////	//Dataチャンクの読み込み
////	ChunkHeader data;
////	file.read((char*)&data, sizeof(data));
////	if (strncmp(data.id, "JUNK", 4) == 0) {
////		file.seekg(data.size, std::ios_base::cur);
////		file.read((char*)&data, sizeof(data));
////	}
////	if (strncmp(data.id, "data", 4) != 0) {
////		assert(0);
////	}
////	char* pBuffer = new char[data.size];
////	file.read(pBuffer, data.size);
////	file.close();
////
////	//returnする為の音声データ
////	SoundData soundData = {};
////	soundData.wfex = format.fmt;
////	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
////	soundData.bufferSize = data.size;
////
////	return soundData;
////}
//
//void SoundUnload(SoundData* soundData) {
//	delete[] soundData->pBuffer;
//	soundData->pBuffer = 0;
//	soundData->bufferSize = 0;
//	soundData->wfex = {};
//}
//
//void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData) {
//	HRESULT result;
//
//	IXAudio2SourceVoice* pSourceVoice = nullptr;
//	result = xAudio2->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
//	assert(SUCCEEDED(result));
//
//	XAUDIO2_BUFFER buf{};
//	buf.pAudioData = soundData.pBuffer;
//	buf.AudioBytes = soundData.bufferSize;
//	buf.Flags = XAUDIO2_END_OF_STREAM;
//
//	result = pSourceVoice->SubmitSourceBuffer(&buf);
//	result = pSourceVoice->Start();
//
//}
//
void CreateSphereVertices(int kSubdivision, float radius, std::vector<VertexData>& vertexData) {
	const float pi = static_cast<float>(std::numbers::pi);
	const float kLonEvery = pi * 2.0f / static_cast<float>(kSubdivision); // 経度分割一つ分の角度
	const float kLatEvery = pi / static_cast<float>(kSubdivision);        // 緯度分割一つ分の角度

	vertexData.clear();
	vertexData.resize(kSubdivision * kSubdivision * 6); // 6頂点（2三角形）×分割数

	for (int latIndex = 0; latIndex < kSubdivision; ++latIndex) {
		float lat = -pi / 2.0f + kLatEvery * latIndex;
		for (int lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLonEvery;

			// 6頂点分（2三角形）の位置とUVを計算
			uint32_t start = (latIndex * kSubdivision + lonIndex) * 6;

			// 各頂点の経度・緯度を計算
			float nextLat = lat + kLatEvery;
			float nextLon = lon + kLonEvery;

			// 頂点座標
			Vector4 v0 = { radius * cos(lat) * cos(lon), radius * sin(lat), radius * cos(lat) * sin(lon), 1.0f };
			Vector4 v1 = { radius * cos(nextLat) * cos(lon), radius * sin(nextLat), radius * cos(nextLat) * sin(lon), 1.0f };
			Vector4 v2 = { radius * cos(lat) * cos(nextLon), radius * sin(lat), radius * cos(lat) * sin(nextLon), 1.0f };
			Vector4 v3 = { radius * cos(nextLat) * cos(nextLon), radius * sin(nextLat), radius * cos(nextLat) * sin(nextLon), 1.0f };

			float f0 = 1.0f - (lat + pi / 2.0f) / pi;
			float f1 = 1.0f - (nextLat + pi / 2.0f) / pi;

			// UV テクスチャ座標
			Vector2 uv0 = { lon / (2.0f * pi), f0 };
			Vector2 uv1 = { lon / (2.0f * pi), f1 };
			Vector2 uv2 = { nextLon / (2.0f * pi), f0 };
			Vector2 uv3 = { nextLon / (2.0f * pi), f1 };

			auto calcNormal = [](const Vector4& v) -> Vector3 {
				float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
				if (len > 0.0001f) {
					return { v.x / len, v.y / len, v.z / len };
				}
				return { 0.0f, 1.0f, 0.0f }; // 万一のため
				};

			// 三角形1
			vertexData[start + 0] = { v0, uv0, calcNormal(v0) };
			vertexData[start + 1] = { v1, uv1, calcNormal(v1) };
			vertexData[start + 2] = { v2, uv2, calcNormal(v2) };
			// 三角形2
			vertexData[start + 3] = { v2, uv2, calcNormal(v2) };
			vertexData[start + 4] = { v1, uv1, calcNormal(v1) };
			vertexData[start + 5] = { v3, uv3, calcNormal(v3) };

			//// 三角形1
			//vertexData[start + 0] = { v0, uv0 };
			//vertexData[start + 1] = { v1, uv1 };
			//vertexData[start + 2] = { v2, uv2 };
			//// 三角形2
			//vertexData[start + 3] = { v2, uv2 };
			//vertexData[start + 4] = { v1, uv1 };
			//vertexData[start + 5] = { v3, uv3 };
		}
	}
}

void DrawSphere(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	const Sphere& sphere, Matrix4x4 wvpMatrix,
	ID3D12Resource* wvpResource, ID3D12Resource* materialResource,
	ID3D12DescriptorHeap* srvDescriptorHeap, D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU,
	ID3D12RootSignature* rootSignature, ID3D12PipelineState* pipelineState,
	D3D12_VERTEX_BUFFER_VIEW vbView, std::vector<VertexData> vertexData
) {

	Matrix4x4* wvpData = nullptr;
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	*wvpData = wvpMatrix;
	wvpResource->Unmap(0, nullptr);

	// パイプライン・ルートシグネチャ設定
	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->SetPipelineState(pipelineState);

	// ディスクリプタヒープ設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap };
	commandList->SetDescriptorHeaps(1, descriptorHeaps);

	// 頂点バッファ設定
	commandList->IASetVertexBuffers(0, 1, &vbView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ルートパラメータ設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);

	// 描画
	commandList->DrawInstanced(static_cast<UINT>(vertexData.size()), 1, 0, 0);

}
