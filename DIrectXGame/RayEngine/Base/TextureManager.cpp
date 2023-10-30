#include "TextureManager.h"
#include "DirectXTex.h"
#include <cassert>
#include "MyLog.h"
#include "MyD3D12Create.h"

using namespace DirectX;

uint32_t TextureManager::Load(const std::string& fileName) {
	return TextureManager::GetInstance()->LoadInternal(fileName);
}

uint32_t TextureManager::Write(Vector4* imageData, const int textureWidth, const int textureHeight) {
	return TextureManager::GetInstance()->WriteInternal(imageData, textureWidth, textureHeight);
}

TextureManager* TextureManager::GetInstance() {
	static TextureManager textureManager;
	return &textureManager;
}

void TextureManager::Initialize(ID3D12Device* device, std::string directoryPath) {
	assert(device);

	device_ = device;
	directoryPath_ = directoryPath;

	sDescriptorHandleIncrementSize_ =
		device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	ResetAll();
}

void TextureManager::ResetAll() {
	HRESULT result = S_FALSE;

	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc = {};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descHeapDesc.NumDescriptors = kNumDescriptors;
	result = device_->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(result));

	indexNextDescriptorHeap_ = 0;

	for (size_t i = 0; i < kNumDescriptors; i++) {
		textures_[i].resource.Reset();
		textures_[i].cpuDescriptorHandleSRV.ptr = 0;
		textures_[i].gpuDescriptorHandleSRV.ptr = 0;
		textures_[i].name.clear();
	}
}

const D3D12_RESOURCE_DESC TextureManager::GetResourceDesc(uint32_t textureHandle) {
	assert(textureHandle < textures_.size());
	Texture& texture = textures_.at(textureHandle);
	return texture.resource->GetDesc();
}

void TextureManager::SetGraphicsRootDescriptorTable(
	ID3D12GraphicsCommandList* commandList, UINT rootParamIndex, uint32_t textureHandle) {
	assert(textureHandle < textures_.size());
	ID3D12DescriptorHeap* ppHeaps[] = { descriptorHeap_.Get() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

	commandList->SetGraphicsRootDescriptorTable(
		rootParamIndex, textures_[textureHandle].gpuDescriptorHandleSRV);

}

uint32_t TextureManager::WriteInternal(Vector4* imageData, const int textureWidth, const int textureHeight) {
	assert((size_t)indexNextDescriptorHeap_ < kNumDescriptors);
	uint32_t handle = indexNextDescriptorHeap_;

	Texture& texture = textures_.at(handle);
	texture.name = "ada";

	
	HRESULT result;
	size_t imageDataCount = size_t(textureWidth * textureHeight);
	//Vector4* imageData_ = new Vector4[imageDataCount];

	//for (size_t i = 0; i < imageDataCount; i++) {
	//	imageData_[i].x = 1.0f;
	//	imageData_[i].y = 0.8f;
	//	imageData_[i].z = 0.5f;
	//	imageData_[i].w = 1.0f;
	//}

	D3D12_RESOURCE_DESC texturesDesc = MyCreate::TexResourceDesc(
		DXGI_FORMAT_R32G32B32A32_FLOAT, (UINT64)textureWidth, (UINT)textureHeight);

	D3D12_HEAP_PROPERTIES heapProps =
		MyCreate::SetTexHeapProp(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	result = device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texturesDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource));
	assert(SUCCEEDED(result));

	result = texture.resource->WriteToSubresource(
		0,
		nullptr,
		imageData,
		(UINT)(sizeof(Vector4) * textureWidth),
		(UINT)(sizeof(Vector4) * imageDataCount));
	assert(SUCCEEDED(result));


	texture.cpuDescriptorHandleSRV = MyCreate::GetCPUDescriptorHandle(
		descriptorHeap_.Get(), handle, sDescriptorHandleIncrementSize_);

	texture.gpuDescriptorHandleSRV = MyCreate::GetGPUDescriptorHandle(
		descriptorHeap_.Get(), handle, sDescriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (UINT)1;

	device_->CreateShaderResourceView(
		texture.resource.Get(),
		&srvDesc,
		texture.cpuDescriptorHandleSRV);

	indexNextDescriptorHeap_ += 1;

	//imageData = imageData_;
	return handle;
}

uint32_t TextureManager::LoadInternal(const std::string& fileName) {
	assert(indexNextDescriptorHeap_ < kNumDescriptors);
	uint32_t handle = indexNextDescriptorHeap_;

	/*for (; textures_.begin() != textures_.end(); textures_.begin()++) {

	}*/

	//textureのコンテナに目的のtextureがあるかしらべる
//	beginからendまでの間で読み込もうとしたtextureがあればその位置を返す
	auto it = std::find_if(textures_.begin(), textures_.end(), [&](const auto& texture) {
		return texture.name == fileName;
		});
	//	もしあればそのtextureのhandleを返す
	if (it != textures_.end()) {
		handle = static_cast<uint32_t>(std::distance(textures_.begin(), it));
		return handle;
	}

	Texture& texture = textures_.at(handle);
	texture.name = fileName;

	bool currentRelative = false;
	if (2 < fileName.size()) {
		currentRelative = (fileName[0] == '.') && (fileName[1] == '/');
	}
	std::string fullPath = currentRelative ? fileName : directoryPath_ + fileName;

	//std::wstring
	wchar_t wfilePath[256]; /*MyLog::ConvertString(fullPath);*/
	MultiByteToWideChar(CP_ACP, 0, fullPath.c_str(), -1, wfilePath, _countof(wfilePath));


	HRESULT result;

	TexMetadata metadata{};
	ScratchImage scratchImg{};
	{
		//ScratchImage image{};
		result = LoadFromWICFile(wfilePath, WIC_FLAGS_NONE, &metadata, scratchImg);
		assert(SUCCEEDED(result));

		ScratchImage mipChain{};

		result = GenerateMipMaps(
			scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
			TEX_FILTER_DEFAULT, 0, mipChain);
		//assert(SUCCEEDED(result));

		if (SUCCEEDED(result)) {
			scratchImg = std::move(mipChain);
			metadata = scratchImg.GetMetadata();
		}
		//scratchImg = mipChain;
		//metadata = scratchImg.GetMetadata();
	}
	metadata.format = MakeSRGB(metadata.format);


	D3D12_RESOURCE_DESC texturesDesc = MyCreate::TexResourceDesc(
		metadata.format, metadata.width, (UINT)metadata.height, (UINT16)metadata.arraySize,
		(UINT16)metadata.mipLevels);

	D3D12_HEAP_PROPERTIES heapProps =
		MyCreate::SetTexHeapProp(D3D12_CPU_PAGE_PROPERTY_WRITE_BACK, D3D12_MEMORY_POOL_L0);

	result = device_->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE,
		&texturesDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texture.resource));
	assert(SUCCEEDED(result));

	for (size_t i = 0; i < metadata.mipLevels; i++) {
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//
		result = texture.resource->WriteToSubresource(
			UINT(i),
			nullptr,
			img->pixels,
			(UINT)img->rowPitch,
			(UINT)img->slicePitch);
		assert(SUCCEEDED(result));
	}

	texture.cpuDescriptorHandleSRV = MyCreate::GetCPUDescriptorHandle(
		descriptorHeap_.Get(), handle, sDescriptorHandleIncrementSize_);

	texture.gpuDescriptorHandleSRV = MyCreate::GetGPUDescriptorHandle(
		descriptorHeap_.Get(), handle, sDescriptorHandleIncrementSize_);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	D3D12_RESOURCE_DESC resDesc = texture.resource->GetDesc();

	srvDesc.Format = resDesc.Format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = (UINT)metadata.mipLevels;

	device_->CreateShaderResourceView(
		texture.resource.Get(),
		&srvDesc,
		texture.cpuDescriptorHandleSRV);

	indexNextDescriptorHeap_++;

	return handle;
}