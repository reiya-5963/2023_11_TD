#include "CommandListManager.h"
#include <cassert>

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE Type) : 
	commandListType_(Type),
	commandQueue_(nullptr),
	fence_(nullptr),
	allocator_(Type)
{}

CommandQueue::~CommandQueue() {
	Finalize();
}

void CommandQueue::Create(ID3D12Device* device) {
	assert(device != nullptr);
	
	// コマンドキューの生成
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};
	commandQueueDesc.Type = commandListType_;
	device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

	allocator_.Create(device);
}

void CommandQueue::Finalize() {
	// コマンドキューがないならスルー
	if (commandQueue_ == nullptr) {
		return;
	}

	// コマンドキューの開放
	commandQueue_->Release();
	commandQueue_ = nullptr;
}

//uint64_t 
void CommandQueue::ExecuteCommandList(ID3D12CommandList* list) {
	((ID3D12GraphicsCommandList*)list)->Close();

	// コマンドリストの実行
	commandQueue_->ExecuteCommandLists(1, &list);
}

ID3D12CommandAllocator* CommandQueue::RequestAllocator(void) {
	return allocator_.RequestAllocator();
}

CommandListManager::CommandListManager() :
	device_(nullptr),
	graphicsQueue_(D3D12_COMMAND_LIST_TYPE_DIRECT)
{}

CommandListManager::~CommandListManager() {
	Finalize();
}

void CommandListManager::Create(ID3D12Device* device) {
	assert(device != nullptr);
	
	device_ = device;

	graphicsQueue_.Create(device_);
}

void CommandListManager::Finalize() {
	graphicsQueue_.Finalize();
}

void CommandListManager::CreateNewCommandList(
	D3D12_COMMAND_LIST_TYPE type,
	ID3D12GraphicsCommandList** list,
	ID3D12CommandAllocator** allocator) 
{
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
		*allocator = graphicsQueue_.RequestAllocator();
		break;
	case D3D12_COMMAND_LIST_TYPE_BUNDLE:
		break;
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		break;
	case D3D12_COMMAND_LIST_TYPE_COPY:
		break;
	default:
		break;
	}
	
	device_->CreateCommandList(1, type, *allocator, nullptr, IID_PPV_ARGS(list));
}
