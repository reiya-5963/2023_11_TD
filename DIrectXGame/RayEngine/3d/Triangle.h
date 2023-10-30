#pragma once
#include "DXCommon.h"
#include <dxcapi.h>
#include <memory>
#include <string>

#include "R_Math.h"
#include "MyLog.h"

/// <summary>
/// スプライト
/// </summary>
class Triangle {
public:
	/// <summary>
	/// 頂点データ
	/// </summary>
	struct VertexPosUv {
		Vector4 pos;
		Vector2 uv;
	};

	/// <summary>
	/// 定数バッファ
	/// </summary>
	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 mat;
	};

public:// 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="directoryPath">shadersファイルまでのパス</param>
	static void StaticInitialize(ID3D12Device* device, int windowWidth, int windowHeight, const std::wstring& directoryPath = L"Resources/");

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// スプライト生成
	/// </summary>
	/// <param name="position">座標</param>
	/// <param name="rotate">回転</param>
	/// <param name="scale">大きさ</param>
	/// <returns>生成されたスプライト</returns>
	static Triangle* Create(uint32_t tectureHandle, Vector3 position,
		Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, Vector2 anchorpoint = { 0.0f, 0.0f });



private:
	//頂点数
	static const int kVertexNum = 3;
	//DXC関連
	static  Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;
	//デバイス
	static ID3D12Device* sDevice_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	//ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	//パイプラインステート
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	//射影行列(消す予定)
	static Matrix4x4 sMatProjection_;

private:// 静的メンバ関数
	/// <summary>
	///DXCの生成
	/// </summary>
	static void CreateDXC();

	/// <summary>
	/// シェーダーコンパイル用の関数
	/// </summary>
	/// <param name="filePath">ファイルの名前</param>
	/// <param name="profile">ファイルのverとか</param>
	/// <param name="dxcUtils">DXC</param>
	/// <param name="dxcCompiler">DXC</param>
	/// <param name="includeHandler">DXC</param>
	/// <returns></returns>
	static IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Triangle();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="position">座標</param>
	/// <param name="rotate">回転</param>
	/// <param name="scale">大きさ</param>
	Triangle(uint32_t textureHandle, Vector3 position, Vector2 size,
		Vector4 color, Vector2 anchorPoint);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>できたかどうか</returns>
	bool Initialize();

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(Vector4 color);
	const Vector4 GetColor() { return color_; }

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(Vector3 rotate);
	const Vector3 GetRotate() { return worldTrans_.rotate; }

	/// <summary>
	/// 大きさ
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetScale(Vector3 scale);
	const Vector3 GetScale() { return worldTrans_.scale; }

	/// <summary>
	/// SRTを丸ごと設定するやつ
	/// </summary>
	/// <param name="translate">SRT構造体</param>
	void SetTranslate(Vector3 translate);
	const Vector3 GetTranslate() { return worldTrans_.translate; }


	/// <summary>
	/// 
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle);
	const uint32_t GetTextureHandle() { return textureHandle_; }

	void SetSize(const Vector2& size);
	const Vector2 GetSize() { return size_; }

	void SetTextureRect(const Vector2& texBase, const Vector2 texSize);


	const Vector2 GetTexSize() { return texSize_; }
	const Vector2 GetTexBase() { return texBase_; }




	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Matrix4x4 viewProMatrix);

private://メンバ変数
	//頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	//頂点バッファマップ
	VertexPosUv* vertexMap_ = nullptr;
	//定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
	//定数バッファマップ
	ConstBufferData* constMap_ = nullptr;

	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//SRT構造体
	Transform worldTrans_{};

	//テクスチャ番号
	UINT textureHandle_ = 0;
	//スプライト幅、高さ
	Vector2 size_ = { 100.0f, 100.0f };
	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f, 0.0f };
	//ワールド行列
	Matrix4x4 matWorld_{};
	//色
	Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };
	//テクスチャ始点
	Vector2 texBase_ = { 0.0f, 0.0f };
	//テクスチャ幅、高さ
	Vector2 texSize_ = { 100.0f, 100.0f };
	//リソース設定
	D3D12_RESOURCE_DESC resourceDesc_;

private://静的メンバ関数
	/// <summary>
	/// 頂点データ転送
	/// </summary>
	void TransferVertices();
};