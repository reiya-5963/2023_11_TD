#pragma once
#include "DXCommon.h"
#include "WinApp.h"
#include "Input.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "DXCManager.h"
#include "GlobalVariables.h"


#include "Sprite.h"
#include "Triangle.h"
#include "Model.h"
#include "ParticleManager.h"

/// <summary>
/// 骨組み
/// </summary>
class Framework {
public: // メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Framework() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 毎フレーム更新前処理
	/// </summary>
	void PreUpdate();

	/// <summary>
	/// 毎フレーム更新後処理
	/// </summary>
	void PostUpdate();

	/// <summary>
	/// 毎フレーム更新処理
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	virtual bool IsEndRequest() { return endRequest_; }
	
	/// <summary>
	/// 実行関数
	/// </summary>
	void Run();

protected: // メンバ変数
	// ウィンドウズAPIクラス
	WinApp* winApp_ = nullptr;

	// DirectXクラス
	DirectXCommon* dxCommon_ = nullptr;

	// 入力管理クラス
	Input* input_ = nullptr;

	// ImGui管理クラス
	ImGuiManager* imGuiManager_ = nullptr;

	// シーン管理クラス
	SceneManager* sceneManager_ = nullptr;

	// 終了リクエスト
	bool endRequest_ = false;
};