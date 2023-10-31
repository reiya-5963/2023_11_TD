//#pragma once
//#include "DXCommon.h"
//#include <dxcapi.h>
//#include <memory>
//#include <string>
//
//#include "MyStruct.h"
//#include "MyLog.h"
//
//class Particle {
//public:
//	struct VertexPosUv {
//		Vector3 pos;
//		Vector2 uv;
//	};                
//
//	/// <summary>
//	/// 定数バッファ
//	/// </summary>
//	struct ConstBufferData {
//		Vector4 color;//色
//		Matrix4x4 mat;//行列
//	};
//
//public:// メンバ関数
//	/// <summary>
//	/// コンストラクタ
//	/// </summary>
//	Particle();
//
//	/// <summary>
//	/// コンストラクタ
//	/// </summary>
//	/// <param name="position">座標</param>
//	/// <param name="rotate">回転</param>
//	/// <param name="scale">大きさ</param>
//	Particle(uint32_t textureHandle, Vector2 position, float rotate, Vector2 size,
//		Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY);
//
//	/// <summary>
//	/// 初期化
//	/// </summary>
//	/// <returns>できたかどうか</returns>
//	bool Initialize();
//
//	/// <summary>
//	/// 色の設定
//	/// </summary>
//	/// <param name="color">色</param>
//	void SetColor(const Vector4& color);
//	const Vector4& GetColor() { return color_; }
//
//	/// <summary>
//	/// 回転
//	/// </summary>
//	/// <param name="rotate">回転</param>
//	void SetRotate(const float& rotate);
//	const float& GetRotate() { return rotation_; }
//
//	/// <summary>
//	/// 大きさ
//	/// </summary>
//	/// <param name="scale">大きさ</param>
//	void SetSize(const Vector2& size);
//	const Vector2& GetSize() { return size_; }
//
//	/// <summary>
//	/// 位置
//	/// </summary>
//	/// <param name="translate">SRT構造体</param>
//	void SetPosition(const Vector2& position);
//	const Vector2& GetTranslate() { return position_; }
//
//	void SetTextureRect(const Vector2& texBase, const Vector2 texSize);
//
//
//	const Vector2 GetTexSize() { return texSize_; }
//	const Vector2 GetTexBase() { return texBase_; }
//
//
//	void SetResourceDesc(D3D12_RESOURCE_DESC resourceDesc) { resourceDesc_ = resourceDesc; }
//
//	/// <summary>
//	/// 描画
//	/// </summary>
//	void Draw(ID3D12GraphicsCommandList* commandList);
//
//private://メンバ変数
//	//頂点バッファ
//	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
//	//頂点バッファマップ
//	VertexPosUv* vertexMap_ = nullptr;
//	//定数バッファ
//	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffer_;
//	//定数バッファマップ
//	ConstBufferData* constMap_ = nullptr;
//	//頂点バッファビュー
//	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
//	//テクスチャ番号
//	UINT textureHandle_ = 0;
//	// Z軸回りの回転角
//	float rotation_ = 0.0f;
//	// 座標
//	Vector2 position_{};
//	// スプライト幅、高さ
//	Vector2 size_ = { 100.0f, 100.0f };
//	//アンカーポイント
//	Vector2 anchorPoint_ = { 0.0f, 0.0f };
//	//ワールド行列
//	Matrix4x4 matWorld_{};
//	//色
//	Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };
//	//左右反転
//	bool isFlipX_ = false;
//	//上下反転
//	bool isFlipY_ = false;
//	//テクスチャ始点
//	Vector2 texBase_ = { 0, 0 };
//	//テクスチャ幅、高さ
//	Vector2 texSize_ = { 100.0f, 100.0f };
//	//リソース設定
//	D3D12_RESOURCE_DESC resourceDesc_;
//private://静的メンバ関数
//	/// <summary>
//	/// 頂点データ転送
//	/// </summary>
//	void TransferVertices();
//};
//
