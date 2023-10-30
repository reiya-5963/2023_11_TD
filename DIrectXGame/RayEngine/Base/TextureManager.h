#pragma once

#include <array>
#include "d3dx12.h"
#include "MyLog.h"
#include "R_Math.h"
#include <wrl.h>

/// <summary>
/// テクスチャマネージャ
/// </summary>
class TextureManager {
public:
	//デスクリプターの数
	static const size_t kNumDescriptors = 256;

	/// <summary>
	/// テクスチャ
	/// </summary>
	struct Texture {
		//テクスチャリソース
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		//シェーダリソースビューのハンドル(CPU)
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandleSRV;
		//シェーダリソースビューのハンドル(GPU)
		D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandleSRV;
		//名前
		std::string name;
	};

	/// <summary>
	/// 
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	static uint32_t Load(const std::string& fileName);

	static uint32_t Write(Vector4* imageData, const int textureWidth, const int textureHeight);

	/// <summary>
	/// 
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();



	/// <summary>
	/// 
	/// </summary>
	/// <param name="device"></param>
	/// <param name="directoryPath"></param>
	void Initialize(ID3D12Device* device, std::string directoryPath = "Resources/");

	/// <summary>
	/// 
	/// </summary>
	void ResetAll();


	//static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	const D3D12_RESOURCE_DESC GetResourceDesc(uint32_t textureHandle);
	

	/// <summary>
	/// 
	/// </summary>
	/// <param name="commandList"></param>
	/// <param name="rootParamIndex"></param>
	/// <param name="textureHandle"></param>
	void SetGraphicsRootDescriptorTable(
		ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle);


private:
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	ID3D12Device* device_;

	UINT sDescriptorHandleIncrementSize_ = 0u;

	std::string directoryPath_;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_;

	uint32_t indexNextDescriptorHeap_ = 0u;

	std::array<Texture, kNumDescriptors> textures_;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="fileName"></param>
	/// <returns></returns>
	uint32_t LoadInternal(const std::string& fileName);

	uint32_t WriteInternal(Vector4* imageData, const int textureWidth, const int textureHeight);
};