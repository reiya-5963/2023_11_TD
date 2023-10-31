#pragma once
#include "DXCommon.h"
#include <dxcapi.h>
#include <memory>
#include <string>

#include "R_Math.h"
#include "MyLog.h"



class ParticleManager {
public:
	struct VertexPosUv {
		Vector3 pos;
		Vector2 uv;
	};

	struct ConstBufferData {
		Vector4 color;
		Matrix4x4 mat;
	};

	struct Particle {
		ConstBufferData constBuff_;
	};
public: // 静的メンバ関数
	static void StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath = L"Resources/");

	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	static void PostDraw();

	static void Create(uint32_t tectureHandle);

	static void CreateInstancingResource();

	static void CreateSRV();
private:
	static const int kVertexNum = 4;
	static const int kParticleMax = 10;

	static ID3D12GraphicsCommandList* sCommandList_;
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> sRootSignature_;
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	static Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
	static ConstBufferData* instancingData_;
	static Matrix4x4 sMatProjection_;
	static D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU_;
	static D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU_;

private:
	//頂点バッファ
	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	//頂点バッファマップ
	static VertexPosUv* vertexMap_;
	//頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	//リソース設定
	static D3D12_RESOURCE_DESC resourceDesc_;

	static UINT textureHandle_;

	//テクスチャ始点
	static Vector2 texBase_ ;
	//テクスチャ幅、高さ
	static Vector2 texSize_;


private://静的メンバ関数
	/// <summary>
	/// 頂点データ転送
	/// </summary>
	static void TransferVertices();

};