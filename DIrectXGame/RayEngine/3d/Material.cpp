#include "DXCommon.h"
#include "MyD3D12Create.h"
#include "Material.h"
#include "TextureManager.h"
#include "DirectXTex.h"
#include <cassert>

Material* Material::Create() {
	Material* instance = new Material;
	instance->Initialize();
	return instance;
}

void Material::LoadTexture(const std::string& directoryPath) {
	// テクスチャなし
	if (textureFilename_.size() == 0) {
		textureFilename_ = "white1x1.png";
	}

	//HRESULT result = S_FALSE;

	// WICテクスチャのロード
	DirectX::TexMetadata metadata{};
	DirectX::ScratchImage scratchImage{};

	// ファイルパスを結合
	string filePath = directoryPath + textureFilename_;

	// テクスチャ読み込み
	textureHandle_ = TextureManager::Load(filePath);

}

void Material::Update() {
	// 定数バッファへデータ転送
	constMap_->ambient = ambient_;
	constMap_->diffuse = diffuse_;
	constMap_->specular = specular_;
	constMap_->alpha = alpha_;
}

void Material::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial, UINT rootParameterIndexTexture) {

	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(
		commandList, rootParameterIndexTexture, textureHandle_);
	// マテリアルの定数バッファをセット
	commandList->SetGraphicsRootConstantBufferView(
		rootParameterIndexMaterial, constBuff_->GetGPUVirtualAddress());

}

void Material::SetGraphicsCommand(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndexMaterial, UINT rootParameterIndexTexture, uint32_t textureHandle) {
	// SRVをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(
		commandList, rootParameterIndexTexture, textureHandle);
	// マテリアルの定数バッファをセット
	commandList->SetGraphicsRootConstantBufferView(
		rootParameterIndexMaterial, constBuff_->GetGPUVirtualAddress());
}

void Material::Initialize() {
	// 定数バッファの作成
	CreateConstantBuffer();
}

void Material::CreateConstantBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferData) + 0xff) & ~0xff);

	// 定数バッファの作成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
	// 定数バッファとのデータリンク
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));
}
