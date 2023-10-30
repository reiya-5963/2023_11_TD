#include "MyD3D12Create.h"
#include <cassert>


D3D12_HEAP_PROPERTIES MyCreate::SetDepthHeapProp(
	D3D12_HEAP_TYPE type,
	UINT creationNodeMask,
	UINT nodeMask) {
	D3D12_HEAP_PROPERTIES result{};

	result.Type = type;
	result.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	result.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	result.CreationNodeMask = creationNodeMask;
	result.VisibleNodeMask = nodeMask;
	return result;
}

D3D12_HEAP_PROPERTIES MyCreate::SetTexHeapProp(
	D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
	D3D12_MEMORY_POOL memoryPoolPreference,
	UINT creationNodeMask,
	UINT nodeMask) {
	D3D12_HEAP_PROPERTIES result{};

	result.Type = D3D12_HEAP_TYPE_CUSTOM;
	result.CPUPageProperty = cpuPageProperty;
	result.MemoryPoolPreference = memoryPoolPreference;
	result.CreationNodeMask = creationNodeMask;
	result.VisibleNodeMask = nodeMask;
	return result;
}

D3D12_CLEAR_VALUE MyCreate::ClearValue(
	DXGI_FORMAT format,
	FLOAT depth,
	UINT8 stencil) {

	D3D12_CLEAR_VALUE result{};

	result.Format = format;
	result.DepthStencil.Depth = depth;
	result.DepthStencil.Stencil = stencil;
	return result;
}



D3D12_RESOURCE_DESC MyCreate::TexResourceDesc(
	DXGI_FORMAT format,
	UINT64 width,
	UINT height,
	UINT16 arraySize,
	UINT16 mipLevels,
	UINT sampleCount,
	UINT sampleQuality,
	D3D12_RESOURCE_FLAGS flags,
	D3D12_TEXTURE_LAYOUT layout,
	UINT64 alignment) {

	D3D12_RESOURCE_DESC result{};
	result.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	result.Alignment = alignment;
	result.Width = width;
	result.Height = height;
	result.DepthOrArraySize = arraySize;
	result.MipLevels = mipLevels;
	result.Format = format;
	result.SampleDesc.Count = sampleCount;
	result.SampleDesc.Quality = sampleQuality;
	result.Layout = layout;
	result.Flags = flags;
	return result;
}



D3D12_CPU_DESCRIPTOR_HANDLE MyCreate::GetCPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap, 
	uint32_t descriptorSize,
	uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE MyCreate::GetGPUDescriptorHandle(
	ID3D12DescriptorHeap* descriptorHeap,
	uint32_t descriptorSize,
	uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

D3D12_RESOURCE_BARRIER MyCreate::PutResourceBarrier(
	ID3D12Resource* backBuffer,
	D3D12_RESOURCE_STATES before,
	D3D12_RESOURCE_STATES after) {

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = backBuffer;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;
	return barrier;
}

D3D12_VIEWPORT MyCreate::SetViewport(
	FLOAT topLeftX, FLOAT topLeftY,
	FLOAT width, FLOAT height,
	FLOAT minDepth, FLOAT maxDepth) {

	D3D12_VIEWPORT result{};
	result.Width = width;
	result.Height = height;
	result.TopLeftX = topLeftX;
	result.TopLeftY = topLeftY;
	result.MinDepth = minDepth;
	result.MaxDepth = maxDepth;
	return result;
}

D3D12_RECT MyCreate::SetRect(
	LONG left, LONG top, LONG right, LONG bottom) {
	
	D3D12_RECT result{};
	result.left = left;
	result.right = right;
	result.top = top;
	result.bottom = bottom;
	return result;
}

D3D12_ROOT_PARAMETER MyCreate::MyInitAsConstantBufferView(
	UINT shaderRagister,
	UINT registerSpace,
	D3D12_SHADER_VISIBILITY visibility) {

	D3D12_ROOT_PARAMETER result{};
	result.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	result.ShaderVisibility = visibility;
	result.Descriptor.ShaderRegister = shaderRagister;
	result.Descriptor.RegisterSpace = registerSpace;
	return result;

}

D3D12_ROOT_PARAMETER MyCreate::MyInitAsDescriptorTable(
	UINT numDescriptorRanges,
	const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
	D3D12_SHADER_VISIBILITY visibility) {
	D3D12_ROOT_PARAMETER result{};
	result.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	result.ShaderVisibility = visibility;
	result.DescriptorTable.NumDescriptorRanges = numDescriptorRanges;
	result.DescriptorTable.pDescriptorRanges = pDescriptorRanges;
	return result;
}

D3D12_DESCRIPTOR_RANGE MyCreate::MyInit(
	D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
	UINT numDescriptors,
	UINT baseShaderRegister,
	UINT registerSpace,
	UINT offsetInDescriptorsFromTableStart) {

	D3D12_DESCRIPTOR_RANGE result{};

	result.BaseShaderRegister = baseShaderRegister;
	result.NumDescriptors = numDescriptors;
	result.RangeType = rangeType;
	result.OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
	result.RegisterSpace = registerSpace;
	return result;
}

D3D12_STATIC_SAMPLER_DESC MyCreate::MySampDescInit(
	UINT shaderRegister,
	D3D12_FILTER filter,
	D3D12_TEXTURE_ADDRESS_MODE addressU,
	D3D12_TEXTURE_ADDRESS_MODE addressV,
	D3D12_TEXTURE_ADDRESS_MODE addressW,
	FLOAT mipLODBias,
	UINT maxAnisotropy,
	D3D12_COMPARISON_FUNC comparisonFunc,
	D3D12_STATIC_BORDER_COLOR borderColor,
	FLOAT minLOD,
	FLOAT maxLOD,
	D3D12_SHADER_VISIBILITY shaderVisibility,
	UINT registerSpace) {

	D3D12_STATIC_SAMPLER_DESC result{};

	result.ShaderRegister = shaderRegister;
	result.Filter = filter;
	result.AddressU = addressU;
	result.AddressV = addressV;
	result.AddressW = addressW;
	result.MipLODBias = mipLODBias;
	result.MaxAnisotropy = maxAnisotropy;
	result.ComparisonFunc = comparisonFunc;
	result.BorderColor = borderColor;
	result.MinLOD = minLOD;
	result.MaxLOD = maxLOD;
	result.ShaderVisibility = shaderVisibility;
	result.RegisterSpace = registerSpace;

	return result;
}


D3D12_RESOURCE_DESC MyCreate::SetBufferResourceDesc(size_t sizeInBytes) {
	D3D12_RESOURCE_DESC resourceDesc{};
	//バッファリソース。テクスチャの場合はまた別の設定をする
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = sizeInBytes;
	//バッファの場合はこれらは1にする決まり
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	//バッファの場合はこれにする決まり
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;


	return resourceDesc;
}

D3D12_HEAP_PROPERTIES MyCreate::SetHeapPro(D3D12_HEAP_TYPE type) {
	D3D12_HEAP_PROPERTIES heapProp{};
	heapProp.Type = type;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;
	return heapProp;
}
