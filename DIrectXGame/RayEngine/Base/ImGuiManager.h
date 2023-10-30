#pragma once

#include "DXCommon.h"
#include "WinApp.h"
#include "imgui.h"

/// <summary>
/// ImGuiマネージャ
/// </summary>
class ImGuiManager {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="winApp"></param>
	/// <param name="dxCommon"></param>
	void Initialize(WinApp* winApp, DirectXCommon* dxCommon);

	/// <summary>
	/// 
	/// </summary>
	void Finalize();

	/// <summary>
	/// 
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGuiの後処理
	/// </summary>
	void End();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
private:
	DirectXCommon* dxCommon_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	const ImGuiManager& operator = (const ImGuiManager&) = delete;



};