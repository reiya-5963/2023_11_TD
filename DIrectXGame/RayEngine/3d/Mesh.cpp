#include "DXCommon.h"
#include "MyD3D12Create.h"
#include "Mesh.h"
#include <cassert>
#include <d3dcompiler.h>
#include "DirectXTex.h"

#pragma comment(lib, "d3dcompiler.lib")


void Mesh::SetName(const std::string& name) {
	this->name_ = name;
}

void Mesh::AddVertex(const VertexPosNormalUv& vertex) {
	vertices_.emplace_back(vertex);
}

void Mesh::AddIndex(unsigned short index) {
	indices_.emplace_back(index);
}

//void Mesh::AddSmoothData(unsigned short indexPosition, unsigned short indexVertex) {
//	smoothData_[indexPosition].emplace_back(indexVertex);
//}

//void Mesh::CalculateSmoothVertexNormals() {
//	auto itr = smoothData_.begin();
//	for (; itr != smoothData_.end(); ++itr) {
//		// 各面用の共通頂点コレクション
//		std::vector<unsigned short>& v = itr->second;
//		// 前頂点の法線を平均する
//		Vector3 normal = {};
//		for (unsigned short index : v) {
//			normal.x += vertices_[index].normal.x;
//			normal.y += vertices_[index].normal.y;
//			normal.z += vertices_[index].normal.z;
//		}
//		normal = R_Math::Normalize({normal.x / (float)v.size(), normal.y / (float)v.size(), normal.z / (float)v.size() });
//		
//		for (unsigned short index : v) {
//			vertices_[index].normal = { normal.x, normal.y, normal.z };
//		}
//	}
//
//}

void Mesh::SetMaterial(Material* material) { this->material_ = material; }

void Mesh::CreateBuffers() {
	HRESULT result;

	UINT sizeVerBuff = static_cast<UINT>(sizeof(VertexPosNormalUv) * vertices_.size());

	//
	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	//
	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(sizeVerBuff);

	//
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertBuff_));
	assert(SUCCEEDED(result));

	//
	VertexPosNormalUv* vertMap = nullptr;
	result = vertBuff_->Map(0, nullptr, (void**)&vertMap);
	if (SUCCEEDED(result)) {
		std::copy(vertices_.begin(), vertices_.end(), vertMap);
		vertBuff_->Unmap(0, nullptr);
	}

	//
	vertexBufferView_.BufferLocation = vertBuff_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeVerBuff;
	vertexBufferView_.StrideInBytes = sizeof(vertices_[0]);

	if (FAILED(result)) {
		assert(0);
		return;
	}

	UINT sizeIndeBuff = static_cast<UINT>(sizeof(unsigned short) * indices_.size());
	//
	resourceDesc.Width = sizeIndeBuff;
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&indexBuff_));
	if (FAILED(result)) {
		assert(0);
		return;
	}

	//
	unsigned short* indexMap = nullptr;
	result = indexBuff_->Map(0, nullptr, (void**)&indexMap);
	if (SUCCEEDED(result)) {
		std::copy(indices_.begin(), indices_.end(), indexMap);
		indexBuff_->Unmap(0, nullptr);
	}
	
	//
	IndexBufferView_.BufferLocation = indexBuff_->GetGPUVirtualAddress();
	IndexBufferView_.Format = DXGI_FORMAT_R16_UINT;
	IndexBufferView_.SizeInBytes = sizeIndeBuff;

}

void Mesh::Draw(
	ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
	UINT rootParameterIndexTexture) {
	// 頂点バッファをセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// インデックスバッファをセット
	commandList->IASetIndexBuffer(&IndexBufferView_);

	// マテリアルのグラフィックスコマンドをセット
	material_->SetGraphicsCommand(commandList, rootParameterIndexMaterial, rootParameterIndexTexture);

	// 描画コマンド
	commandList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}

void Mesh::Draw(
	ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial,
	UINT rootParameterIndexTexture, uint32_t textureHandle) {
	// 頂点バッファをセット
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// インデックスバッファをセット
	commandList->IASetIndexBuffer(&IndexBufferView_);

	// マテリアルのグラフィックスコマンドをセット
	material_->SetGraphicsCommand(commandList, rootParameterIndexMaterial, rootParameterIndexTexture, textureHandle);

	// 描画コマンド
	commandList->DrawIndexedInstanced((UINT)indices_.size(), 1, 0, 0, 0);
}
