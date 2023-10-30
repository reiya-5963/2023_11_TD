#pragma once
#include <d3d12.h>
#include <cstdint>

class MyCreate {
public:

	static D3D12_RESOURCE_DESC SetBufferResourceDesc(size_t sizeInBytes);

	static D3D12_HEAP_PROPERTIES SetHeapPro(D3D12_HEAP_TYPE type);

	static D3D12_HEAP_PROPERTIES SetDepthHeapProp(
		D3D12_HEAP_TYPE type,
		UINT creationNodeMask = 1,
		UINT nodeMask = 1);

	static D3D12_HEAP_PROPERTIES SetTexHeapProp(
		D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
		D3D12_MEMORY_POOL memoryPoolPreference,
		UINT creationNodeMask = 1,
		UINT nodeMask = 1);

	static D3D12_CLEAR_VALUE ClearValue(
		DXGI_FORMAT format,
		FLOAT depth,
		UINT8 stencil);	

	static D3D12_RESOURCE_DESC TexResourceDesc(
		DXGI_FORMAT format,
		UINT64 width,
		UINT height,
		UINT16 arraySize = 1,
		UINT16 mipLevels = 0,
		UINT sampleCount = 1,
		UINT sampleQuality = 0,
		D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
		D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		UINT64 alignment = 0);
	

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap,
		uint32_t descriptorSize,
		uint32_t index);

	static D3D12_RESOURCE_BARRIER PutResourceBarrier(ID3D12Resource* backBuffer, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);


	static D3D12_ROOT_PARAMETER MyInitAsConstantBufferView(
		UINT shaderRagister,
		UINT registerSpace = 0,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	static D3D12_ROOT_PARAMETER MyInitAsDescriptorTable(
		UINT numDescriptorRanges,
		const D3D12_DESCRIPTOR_RANGE* pDescriptorRanges,
		D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

	static D3D12_DESCRIPTOR_RANGE MyInit(
		D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
		UINT numDescriptors,
		UINT baseShaderRegister,
		UINT registerSpace = 0,
		UINT offsetInDescriptorsFromTableStart =
		D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND);

	static D3D12_VIEWPORT SetViewport(
		FLOAT topLeftX, FLOAT topLeftY,
		FLOAT width, FLOAT height,
		FLOAT minDepth = 0.0f, FLOAT maxDepth = 1.0f);

	static D3D12_RECT SetRect(LONG left, LONG top, LONG right, LONG bottom);

	static D3D12_STATIC_SAMPLER_DESC MySampDescInit(
		UINT shaderRegister,
		D3D12_FILTER filter = D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE addressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE addressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE addressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		FLOAT mipLODBias = 0,
		UINT maxAnisotropy = 16,
		D3D12_COMPARISON_FUNC comparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR borderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE,
		FLOAT minLOD = 0.f,
		FLOAT maxLOD = D3D12_FLOAT32_MAX,
		D3D12_SHADER_VISIBILITY shaderVisibility = D3D12_SHADER_VISIBILITY_ALL,
		UINT registerSpace = 0);
private:
};