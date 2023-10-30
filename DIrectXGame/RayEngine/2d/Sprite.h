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
class Sprite {
public:
	enum class BlendMode {
		kNone,
		kAdd,
		kSubtract,
		kMultiply,
		kScreen,
		BlendModeCount
	};

public:
	struct VertexPosUv {
		Vector3 pos;
		Vector2 uv;
	};

	/// <summary>
	/// 定数バッファ
	/// </summary>
	struct ConstBufferData {
		Vector4 color;//色
		Matrix4x4 mat;//行列
	};

public:// 静的メンバ関数

	/// <summary>
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="directoryPath">shadersファイルまでのパス</param>
	static void StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath = L"Resources/");

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
	static Sprite* Create(uint32_t tectureHandle, Vector2 position, float rotate,
		Vector4 color = {1.0f, 1.0f, 1.0f, 1.0f}, Vector2 anchorpoint = {0.0f, 0.0f}, bool isFlipX = false,
		bool isFlipY = false);
	


private:
	//頂点数
	static const int kVertexNum = 4;
	//コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	//ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	//パイプラインステート(ブレンドモードの数分)
	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(BlendMode::BlendModeCount)>sPipelineStates_;
	//射影行列(消す予定)
	static Matrix4x4 sMatProjection_;

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Sprite();

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="position">座標</param>
	/// <param name="rotate">回転</param>
	/// <param name="scale">大きさ</param>
	Sprite(uint32_t textureHandle, Vector2 position, float rotate, Vector2 size,
		Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>できたかどうか</returns>
	bool Initialize();

	/// <summary>
	/// 色の設定
	/// </summary>
	/// <param name="color">色</param>
	void SetColor(const Vector4& color);
	const Vector4& GetColor() { return color_; }

	/// <summary>
	/// 回転
	/// </summary>
	/// <param name="rotate">回転</param>
	void SetRotate(const float &rotate);
	const float& GetRotate() { return rotation_; }

	/// <summary>
	/// 大きさ
	/// </summary>
	/// <param name="scale">大きさ</param>
	void SetSize(const Vector2& size);
	const Vector2& GetSize() { return size_; }

	/// <summary>
	/// 位置
	/// </summary>
	/// <param name="translate">SRT構造体</param>
	void SetPosition(const Vector2& position);
	const Vector2& GetTranslate() { return position_; }


	/// <summary>
	/// 
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle);
	const uint32_t GetTextureHandle() { return textureHandle_; }


	void SetTextureRect(const Vector2& texBase, const Vector2 texSize);


	const Vector2 GetTexSize() { return texSize_; }
	const Vector2 GetTexBase() { return texBase_; }

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(BlendMode blendMode = BlendMode::kNone);

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
	//テクスチャ番号
	UINT textureHandle_ = 0;
	// Z軸回りの回転角
	float rotation_ = 0.0f;
	// 座標
	Vector2 position_{};
	// スプライト幅、高さ
	Vector2 size_ = { 100.0f, 100.0f };
	//アンカーポイント
	Vector2 anchorPoint_ = { 0.0f, 0.0f };
	//ワールド行列
	Matrix4x4 matWorld_{};
	//色
	Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };
	//左右反転
	bool isFlipX_ = false;
	//上下反転
	bool isFlipY_ = false;
	//テクスチャ始点
	Vector2 texBase_ = { 0, 0 };
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