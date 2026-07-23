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
#include <imgui.h>

#include "Vector.h"
#include "Matrix.h"
#include "MathFunc.h"
#include "Input.h"
#include "Window.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "ModelCommon.h"
#include "Model.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ModelManager.h"
#include "SrvManager.h"
#include "ImguiManager.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "xaudio2.lib")


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);



struct Sphere {
	Vector3 center;
	float radius;
};

//struct TransformationMatrix {
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

//struct DirectionalLight {
//	Vector4 color;
//	Vector3 direction;
//	float intensity;
//};

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
	SpriteCommon* spriteCommon = nullptr;
	Sprite* sprite = nullptr;
	ModelCommon* modelCommon = nullptr;
	Model* model = nullptr;
	Object3dCommon* object3dCommon = nullptr;
	Object3d* object3d = nullptr;
	Object3d* object3d2 = nullptr;
	SrvManager* srvManager = nullptr;
	ImGuiManager* imguiManager = nullptr;

	//初期化
	window = new Window();
	dxCommon = new DirectXCommon();
	spriteCommon = new SpriteCommon();
	sprite = new Sprite();
	modelCommon = new ModelCommon();
	model = new Model();
	object3dCommon = new Object3dCommon();
	object3d = new Object3d();
	object3d2 = new Object3d();
	Camera* camera = new Camera();
	srvManager = SrvManager::GetInstance();
	imguiManager = new ImGuiManager();

	
	ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device * device, int32_t width, int32_t height);
	void CreateSphereVertices(int kSubdivision, float radius, std::vector<VertexData>&vertexData);
	
	////変数の宣言
	//HRESULT hr;
	Transform transform{ {1.0f,1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform transformSprite{ {1.0f,1.0f,1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f,}, {0.0f,0.0f, -5.0f} };
	Sphere sphere = { {0.0f, 0.0f, 0.0f}, 1.0f };
	Transform uvTransformSprite{ {1.0f,1.0f,1.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, };
	//std::vector<VertexData> sphereVertices;
	//Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, static_cast<float>(Window::kClientWidth) / static_cast<float>(Window::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	//Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
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
	//Matrix4x4 worldViewProjectionMatrixSphere = Multiply(worldMatrixSphere, Multiply(viewMatrix, projectionMatrix));

	//UVTransform用の行列
	Matrix4x4 uvTransformMatrix = MakeAffineMatrix(uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);

	//Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	//IXAudio2MasteringVoice* masterVoice;
	//HRESULT result;

	window->Initialize();
	dxCommon->Initialize(window);
	srvManager->Initialize(dxCommon);
	TextureManager::GetInstance()->Initialize(dxCommon, srvManager);
	spriteCommon->Initialize(dxCommon);
	sprite->Initialize(spriteCommon, "resources/uvChecker.png");
	ModelManager::GetInstance()->Initialize(dxCommon);
	ModelManager::GetInstance()->LoadModel("axis.obj"); //.objからモデルを読み込む
	modelCommon->Initialize(dxCommon);
	model->initialize(modelCommon, "resources", "axis.obj");
	object3dCommon->Initialize(dxCommon);

	object3d->Initialize(object3dCommon);
	object3d->SetModel(model);
	object3d->SetCamera(camera);

	object3d2->Initialize(object3dCommon);
	object3d2->SetModel(model);
	object3d2->SetCamera(camera);
	object3d2->SetTranslate({ 1.0f, 1.0f, 0.0f });

	imguiManager->Initialize(window, dxCommon);

	//camera->SetRotate({ 0.0f,0.0f,0.0f });
	//camera->SetTranslate({ 0.0f,0.0f,0.0f });
	//object3dCommon->SetDefaultCamera(camera);

	//result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	//result = xAudio2->CreateMasteringVoice(&masterVoice);

	Vector2 spritePos{};

#ifdef _DEBUG

	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();

		debugController->SetEnableGPUBasedValidation(TRUE);
	}

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
		//worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
		//wvpData->WVP = worldViewProjectionMatrix;
		//wvpData->World = worldMatrix;
		//*wvpData = worldViewProjectionMatrix;

		worldMatrixSprite = MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
		worldViewProjectionMatrixSprite = Multiply(worldMatrixSprite, Multiply(viewMatrixSprite, projectionMatrixSprite));
		//*transformationMatrixDataSprite = worldViewProjectionMatrixSprite;

		uvTransformMatrix = MakeAffineMatrix(uvTransformSprite.scale, uvTransformSprite.rotate, uvTransformSprite.translate);
		//materialDataSprite->uvTransform = uvTransformMatrix;

		if (input->TriggerKey(DIK_0)) {
			OutputDebugStringA("Hit 0\n");
		}

#ifdef USE_IMGUI
		imguiManager->Begin();
		ImGui::DragFloat("spritePosX", &spritePos.x);
		//ImGui::Text("Hello, world %d", 123);
		imguiManager->End();
#endif
		sprite->SetPosition(spritePos);

		srvManager->PreDraw();
		dxCommon->PreDraw();

		spriteCommon->CreatePrimitiveTopology();
		sprite->Update();
		sprite->Draw();
		object3dCommon->CreatePrimitiveTopology();
		camera->Update();
		object3d->Update();
		object3d2->Update();
		object3d->Draw();
		object3d2->Draw();

		
		imguiManager->Draw();
		dxCommon->PostDraw();
		//TextureManager::GetInstance()->Finalize();

	}

	
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

	TextureManager::GetInstance()->Finalize();
	ModelManager::GetInstance()->Finalize();
	imguiManager->Finalize();
	delete input;
	SrvManager::GetInstance()->Finalize();
	delete window;
	delete dxCommon;

	return 0;
}

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
