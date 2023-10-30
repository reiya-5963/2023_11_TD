#include "Framework.h"

void Framework::Initialize() {
 #pragma region WindowAPIクラス初期化
	// WinAppのシングルトンの取得
	winApp_ = WinApp::GetInstance();

	// ウィンドウの作成
	winApp_->Initialize(L"LE2A_22_ワタナベエンジン");
 #pragma endregion

 #pragma region DirectX汎用クラス初期化
	// dxCommonのシングルトンの取得
	dxCommon_ = DirectXCommon::GetInstance();

	// directX関連の初期化処理
	dxCommon_->Initialize(winApp_);
 #pragma endregion

	// DXC生成
	DXCManager::GetInstance()->CreateDXC();

 #pragma region 機能の初期化

 #pragma region ImGui管理クラスの初期化
	// ImGuiManagerのシングルトンの取得
	imGuiManager_ = ImGuiManager::GetInstance();

	// ImGuiの初期化処理
	imGuiManager_->Initialize(winApp_, dxCommon_);
 #pragma endregion

 #pragma region 入力クラスの初期化
	// 入力の初期化
	input_ = Input::GetInstance();
	input_->Initialize();
 #pragma endregion

 #pragma region テクスチャ管理クラスの初期化
	// TextureManagerの初期化処理 (シングルトン)
	TextureManager::GetInstance()->Initialize(dxCommon_->GetDevice());
	// 1x1の白画像を読み込む
	TextureManager::Load("white1x1.png");
 #pragma endregion

 #pragma region スプライト, モデルの静的初期化

	// スプライトの静的初期化
	Sprite::StaticInitialize(WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 三角形の静的初期化
	Triangle::StaticInitialize(dxCommon_->GetDevice(), WinApp::kWindowWidth, WinApp::kWindowHeight);

	// 3dモデルの静的初期化
	Model::StaticInitialize();

	//ParticleManager::StaticInitialize(WinApp::kWindowWidth, WinApp::kWindowHeight);
 #pragma endregion

 #pragma endregion

	// シーン管理クラスの初期化処理
	sceneManager_ = SceneManager::GetInstance();
	sceneManager_->Initialize();

	// グローバル変数の読み込み
	GlobalVariables::GetInstance()->LoadFiles();
}

void Framework::Finalize() {
	// シーン管理クラスの終了処理
	sceneManager_->Finalize();

	// ImGuiのRelease処理
	imGuiManager_->Finalize();

	// ウィンドウの破棄の処理
	winApp_->Finalize();
}

void Framework::PreUpdate() {
	// 入力関連の毎フレーム処理
	input_->Update();

	// ImGuiの前処理
	imGuiManager_->Begin();

	// グローバル変数の更新
	GlobalVariables::GetInstance()->Update();
}

void Framework::PostUpdate() {
	// シーン管理クラスの毎フレーム更新処理
	sceneManager_->Update();

	// ImGuiの後処理
	imGuiManager_->End();

	// もしウィンドウを破棄するならゲーム終了リクエストを送る
	if (winApp_->ProcessMessage())
		endRequest_ = true;
}

void Framework::Update() {
	// 毎フレーム更新前処理
	PreUpdate();

	// 毎フレーム更新後処理
	PostUpdate();
}

void Framework::Run() {
	// 初期化処理
	Initialize();

	// ゲームループ
	while (true) {
		// もしウィンドウが破棄されたらループを抜ける
		if (IsEndRequest()) {
			break;
		}
		else {
			// 毎フレーム更新処理
			Update();

			// 描画処理
			Draw();
		}
	}

	// 終了処理
	Finalize();
}
