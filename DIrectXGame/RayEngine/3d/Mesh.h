#pragma once

#include "Material.h"
#include "R_Math.h"
#include <Windows.h>
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>


class Mesh {
public:
	struct VertexPosNormalUv {
		Vector3 pos;
		Vector3 normal;
		Vector2 uv;
	};

public:

	const std::string& GetName() { return name_; }

	void SetName(const std::string& name);

	void AddVertex(const VertexPosNormalUv& vertex);

	void AddIndex(unsigned short index);

	inline size_t GetVertexCount() { return vertices_.size(); }

	//void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	//void CalculateSmoothVertexNormals();

	Material* GetMaterial() { return material_; }

	void SetMaterial(Material* material);

	void CreateBuffers();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() { return vertexBufferView_; }
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() { return IndexBufferView_; }

	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
		UINT rootParameterIndexTexture);

	void Draw(
		ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
		UINT rootParameterIndexTexture, uint32_t textureHandle);

	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

private:
	// 名前
	std::string name_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW IndexBufferView_ = {};
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;
	// 頂点法線スムージング用データ
	//std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;
	// マテリアル
	Material* material_ = nullptr;
};