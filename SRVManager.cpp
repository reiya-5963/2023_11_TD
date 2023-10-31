#include "SRVManager.h"
#include <cassert>
Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> SRVManager::srvHeap_;

SRVManager* SRVManager::GetInstance() {
	static SRVManager instance;
	return &instance;
}

void SRVManager::Initialize(ID3D12Device* device, bool shaderVisible) {
	//ディスクリプタヒープの生成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;	//レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = 256 * 2;					//ダブルバッファ用に二つ。多くても別にかまわない
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&srvHeap_));
	//ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

}
