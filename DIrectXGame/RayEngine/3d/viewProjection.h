#pragma once

#include "WinApp.h"
#include "R_Math.h"
#include <d3d12.h>
#include <wrl.h>

/// <summary>
/// 定数バッファ用データ構造体
/// </summary>
struct ConstBufferDataViewProjection {
	Matrix4x4 view;				// ワールド変換行列 -> ビュー変換行列
	Matrix4x4 projection;		// ビュー変換行列 -> プロジェクション変換行列
	Vector3 cameraPos;			// カメラ座標 (ワールド座標)
};

struct ViewProjection {
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// マッピング済みアドレス
	ConstBufferDataViewProjection* constMap = nullptr;

#pragma region ビュー行列の設定
	// 
	Vector3 translation_ = { 0.0f, 0.0f, -10.0f };
	// 
	Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
	// 
	Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
#pragma endregion

#pragma region 射影行列の設定
	// 垂直方向視野角
	float fovAngleY = 45.0f * 3.1415926535f / 180.0f;
	// ビューポートのアスペクト比
	float aspectRatio = float(WinApp::kWindowWidth) / float(WinApp::kWindowHeight);
	// 深度限界(手前側)
	float nearZ = 0.1f;
	// 深度限界(奥側)
	float farZ = 1000.0f;
#pragma endregion
	// ビュー行列
	Matrix4x4 matView;
	// 射影行列
	Matrix4x4 matProjection;

	/// <summary>
	/// 
	/// </summary>
	void Initialize();

	/// <summary>
	/// 
	/// </summary>
	void CreateConstBuffer();

	/// <summary>
	/// 
	/// </summary>
	void Map();

	/// <summary>
	/// 
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// 
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// 
	/// </summary>
	void UpdateProjectionMatrix();
};