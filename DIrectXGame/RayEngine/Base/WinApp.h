#pragma once
#include <Windows.h>
#include <cstdint>

/// <summary>
/// ウィンドウズアプリケーション
/// </summary>
class WinApp {
public: //静的メンバ変数
	static const int kWindowWidth = 1280;	//横幅
	static const int kWindowHeight = 720;	//縦幅
	//ウィンドウクラス名
	static const wchar_t kWindowClassName[];

public: //静的メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static WinApp* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd">ウィンドウハンドル</param>
	/// <param name="msg">メッセージ番号</param>
	/// <param name="wparam">メッセージ情報1</param>
	/// <param name="lparam">メッセージ情報2</param>
	/// <returns>できたかどうか</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	/// <param name="title">ウィンドウタイトル</param>
	/// <param name="windowStyle">ウィンドウの初期スタイル</param>
	/// <param name="clientWindth">ウィンドウのクライアント領域の初期幅</param>
	/// <param name="clientHeight">ウィンドウのクライアント領域の初期高さ</param>
	void Initialize(
		const wchar_t* title = L"ReyEngine", UINT windowStyle = WS_OVERLAPPEDWINDOW,
		int32_t clientWindth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	/// <summary>
	/// ゲームウィンドウの破棄
	/// </summary>
	void Finalize();


	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns>終了したかどうか</returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	HWND GetHwnd() const { return hwnd_; }

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	HINSTANCE GetHInstance() const { return wndClassEx_.hInstance; }

private: //メンバ関数
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator = (const WinApp&) = delete;

private://メンバ変数
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	WNDCLASSEX wndClassEx_{}; // ウィンドウクラス
	UINT windowStyle_;
};