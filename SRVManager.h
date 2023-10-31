#pragma once
#include "DXCommon.h"
#include <dxcapi.h>
#include <memory>
#include <string>

#include "R_Math.h"
#include "MyLog.h"

class SRVManager {
public:
	static SRVManager* GetInstance();

	static void Initialize(ID3D12Device* device, bool shaderVisible);
	

	ID3D12DescriptorHeap* GetSRVDescHeap() { return srvHeap_.Get(); }
private:
	static const int kNumTexDescriptorHeap = 256;

	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
};

