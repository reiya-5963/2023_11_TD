#pragma once
#include <d3d12.h>
#include <wrl.h>

class CommandAllocator {
public:
	CommandAllocator(D3D12_COMMAND_LIST_TYPE type);
	~CommandAllocator();

	void Create(ID3D12Device* device);
	void Finalize();

	ID3D12CommandAllocator* RequestAllocator();

	size_t Size() { return 1; }
private:
	const D3D12_COMMAND_LIST_TYPE commandListType_;

	ID3D12Device* device_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;

};

