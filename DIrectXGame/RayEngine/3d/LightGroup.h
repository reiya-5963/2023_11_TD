#pragma once

#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include "R_Math.h"

#include "DirectionalLight.h"

class LightGroup {
public:
	//
	static const int kDirectLightNum = 3;
	//
	static const int kPointLightNum = 3;
	//
	static const int kSpotLightNum = 3;
	// 
	static const int kCircleShadowNum = 1;
public: // サブクラス
	struct ConstBufferData {
		// 環境光の色
		Vector3 ambientColor;
		float pad1;
		// 平行光源用
		DirectionalLight::ConstBufferData directLights[kDirectLightNum];
	};
public: // 静的メンバ関数
	static LightGroup* Create();

public: // メンバ関数
	void Initialize();

	void Update();

	void Draw(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

	void TransferConstBuffer();

	void DefaultLightSetting();

	void SetAmbientColor(const Vector3& color);

	void SetDirectLightActive(int index, bool active);

	void SetDirectLightDir(int index, const Vector3& lightDir);

	void SetDirectLightColor(int index, const Vector3& lightColor);

private: // メンバ変数
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// 定数バッファのマップ
	ConstBufferData* constMap_ = nullptr;

	// 環境光の色
	Vector3 ambientColor_ = { 1.0f, 1.0f, 1.0f };

	// 平行光源の配列
	DirectionalLight directLights_[kDirectLightNum];

	// ダーティフラグ
	bool dirty_ = false;
};