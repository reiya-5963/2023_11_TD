#include "WinApp.h"
#include "imgui_impl_win32.h"
#pragma comment(lib, "winmm.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

const wchar_t WinApp::kWindowClassName[] = L"ReyEngine";

WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	//WinApp* app = reinterpret_cast<WinApp*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}

	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}


void WinApp::Initialize(
	const wchar_t* title, UINT windowStyle,
	int32_t clientWindth, int32_t clientHeight) {
	
	// 引数を代入
	windowStyle_ = windowStyle;

	//
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	// システムタイマーの分解能を上げる
	timeBeginPeriod(1);


	//ウィンドウクラスの設定
	wndClassEx_.cbSize = sizeof(WNDCLASSEX);			// このウィンドウクラスのサイズ
	wndClassEx_.lpfnWndProc = (WNDPROC)WindowProc;		// ウィンドウプロシージャを設定
	wndClassEx_.lpszClassName = kWindowClassName;		// ウィンドウクラス名
	wndClassEx_.hInstance = GetModuleHandle(nullptr);	// ウィンドウハンドル
	wndClassEx_.hCursor = LoadCursor(NULL, IDC_ARROW);	// カーソル設定

	// ウィンドウクラスをOSに登録する
	RegisterClassEx(&wndClassEx_);

	// ウィンドウサイズ = { x, y, 横幅, 縦幅 }
	RECT wrc = { 0, 0, clientWindth, clientHeight };
	
	// 自動でサイズを補正する
	AdjustWindowRect(&wrc, windowStyle_, false);

	// ウィンドウオブジェクトの生成
	hwnd_ = CreateWindow(
		wndClassEx_.lpszClassName, // クラス名
		title,                   // タイトルバーの文字
		windowStyle_,            // タイトルバーと境界線があるウィンドウ
		CW_USEDEFAULT,           // 表示X座標（OSに任せる）
		CW_USEDEFAULT,           // 表示Y座標（OSに任せる）
		wrc.right - wrc.left,    // ウィンドウ横幅
		wrc.bottom - wrc.top,    // ウィンドウ縦幅
		nullptr,                 // 親ウィンドウハンドル
		nullptr,                 // メニューハンドル
		wndClassEx_.hInstance,     // 呼び出しアプリケーションハンドル
		nullptr);                // オプション

	// ウィンドウを表示状態にする
	ShowWindow(hwnd_, SW_SHOW);

}

void WinApp::Finalize() {
	// ウィンドウクラスの登録解除
	UnregisterClass(wndClassEx_.lpszClassName, wndClassEx_.hInstance);

	CoUninitialize();
}


bool WinApp::ProcessMessage() {
	MSG msg{};//メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
		return true;
	}

	return false;
}