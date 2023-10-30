#pragma once

#include "R_Math.h"
#include <d3d12.h>
#include <string>
#include <wrl.h>

/// <summary>
/// マテリアル
/// </summary>
class Material {
public:
	struct ConstBufferData {
		Vector3 ambient;	// アンビエント係数
		float pad1;			// 
		Vector3 diffuse;	// ディフューズ係数
		float pad2;			// 
		Vector3 specular;	// スペキュラー係数
		float alpha;		// アルファ値
	};
public: // 静的メンバ関数
	/// <summary>
	/// マテリアル生成
	/// </summary>
	/// <returns></returns>
	static Material* Create();

public:
	std::string name_;				// マテリアル名
	Vector3 ambient_;				// アンビエント影響度
	Vector3 diffuse_;				// ディフューズ影響度
	Vector3 specular_;				// スペキュラー影響度
	float alpha_;					// アルファ値
	std::string textureFilename_;	// テクスチャファイル名

public:
	/// <summary>
	/// 定数バッファの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetConstantBuffer() { return constBuff_.Get(); }
	
	/// <summary>
	/// テクスチャ読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	void LoadTexture(const std::string& directoryPath);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// グラフィックスコマンドのセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParameterIndexMaterial">マテリアルのルートパラメータ番号</param>
	/// <param name="rootParameterIndexTexture">テクスチャのルートパラメータ番号</param>
	void SetGraphicsCommand(
		ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
		UINT rootParameterIndexTexture);

	/// <summary>
	/// グラフィックスコマンドのセット
	/// </summary>
	/// <param name="commandList">コマンドリスト</param>
	/// <param name="rootParameterIndexMaterial">マテリアルのルートパラメータ番号</param>
	/// <param name="rootParameterIndexTexture">テクスチャのルートパラメータ番号</param>
	/// <param name="textureHandle">差し替えるテクスチャハンドル</param>
	void SetGraphicsCommand(
		ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
		UINT rootParameterIndexTexture, uint32_t textureHandle);

	/// <summary>
	/// テクスチャハンドルの取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetTextureHandle() { return textureHandle_; }

private: // メンバ変数
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// 定数バッファのマップ
	ConstBufferData* constMap_ = nullptr;
	// テクスチャハンドル
	uint32_t textureHandle_ = 0;
private: // メンバ関数
	// コンストラクタ
	Material() {
		ambient_ = { 0.3f, 0.3f, 0.3f };
		diffuse_ = { 0.0f, 0.0f ,0.0f };
		specular_ = { 0.0f, 0.0f, 0.0f };
		alpha_ = 1.0f;
	}

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 定数バッファの生成
	/// </summary>
	void CreateConstantBuffer();

};