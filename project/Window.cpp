
#include "Window.h"
#include <cstdint>
#include "externals/imgui/imgui.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Window::Initialize() {
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//クライアント領域のサイズ
	//const int32_t kClientWidth = 1280;
	//const int32_t kClientHeight = 720;

	//WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc; //ウィンドウプロシージャ
	wc.lpszClassName = L"CG2WindowClass"; //ウィンドウクラス名
	wc.hInstance = GetModuleHandle(nullptr); //インスタンスハンドル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); //カーソル
	RegisterClass(&wc); //ウィンドウクラスを登録する

	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = { 0,0, kClientWidth, kClientHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	 hwnd = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	ShowWindow(hwnd, SW_SHOW);
}

bool Window::ProcessMessage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}

void Window::Finalize() {
	CloseWindow(hwnd);
	CoUninitialize();
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {

		//ウィンドウが破棄された
	case  WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);

		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
