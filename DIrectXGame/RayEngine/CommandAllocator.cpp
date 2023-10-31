#include "CommandAllocator.h"

CommandAllocator::CommandAllocator(D3D12_COMMAND_LIST_TYPE type) :
	commandListType_(type),
	device_(nullptr)
{}

CommandAllocator::~CommandAllocator() {
	Finalize();
}

void CommandAllocator::Create(ID3D12Device* device) {
	device_ = device;
}

void CommandAllocator::Finalize() {
	commandAllocator_->Release();
}

ID3D12CommandAllocator* CommandAllocator::RequestAllocator() {
	ID3D12CommandAllocator* allocator = nullptr;

	if (allocator == nullptr) {
		device_->CreateCommandAllocator(commandListType_, IID_PPV_ARGS(&allocator));
	}

	return allocator;
}
