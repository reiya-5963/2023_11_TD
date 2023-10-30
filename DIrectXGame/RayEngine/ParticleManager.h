//#pragma once
//#include "DXCommon.h"
//#include <dxcapi.h>
//#include <memory>
//#include <string>
//#include <list>
//#include "MyStruct.h"
//#include "MyLog.h"
//
//class Particle;
//struct Particle::ConstBufferData;
//
//
//class ParticleManager {
//public:
//	enum class BlendMode {
//		kNone,
//		kAdd,
//		kSubtract,
//		kMultiply,
//		kScreen,
//		BlendModeCount
//	};
//
//
//	const uint32_t kParticleNum = 10;
//public:
//
//	/// <summary>
//	/// 静的初期化
//	/// </summary>
//	/// <param name="device">デバイス</param>
//	/// <param name="directoryPath">shadersファイルまでのパス</param>
//	static void StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath = L"Resources/");
//
//	/// <summary>
//	/// 描画前処理
//	/// </summary>
//	/// <param name="commandList">描画コマンドリスト</param>
//	static void PreDraw(ID3D12GraphicsCommandList* commandList);
//
//	/// <summary>
//	/// 描画後処理
//	/// </summary>
//	static void PostDraw();
//
//	static void Draw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode);
//
//	/// <summary>
//	/// スプライト生成
//	/// </summary>
//	/// <param name="position">座標</param>
//	/// <param name="rotate">回転</param>
//	/// <param name="scale">大きさ</param>
//	/// <returns>生成されたスプライト</returns>
//	static void AddParticle(Particle* particle);
//	/// <summary>
//	/// 
//	/// </summary>
//	/// <param name="textureHandle"></param>
//	void SetTextureHandle(uint32_t textureHandle);
//	const uint32_t GetTextureHandle() { return textureHandle_; }
//
//	static Matrix4x4 GetMatProjection() { return sMatProjection_; }
//
//	void Initialize();
//private: // 
//	//頂点数
//	static const int kVertexNum = 4;
//	//コマンドリスト
//	static ID3D12GraphicsCommandList* sCommandList_;
//	//ルートシグネチャ
//	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
//	//パイプラインステート(ブレンドモードの数分)
//	static std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(BlendMode::BlendModeCount)>sPipelineStates_;
//	//射影行列(消す予定)
//	static Matrix4x4 sMatProjection_;
//
//	static std::list<Particle*> particles_;	
//	
//	//頂点バッファ
//	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResourceBuffer_;
//	//頂点バッファマップ
//	Particle::ConstBufferData* instancingData_ = nullptr;
//
//private://静的メンバ関数
//	//テクスチャ番号
//	static UINT textureHandle_;
//	D3D12_RESOURCE_DESC resourceDesc_;
//};
//
