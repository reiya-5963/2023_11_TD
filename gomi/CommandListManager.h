#pragma once
#include <d3d12.h>
#include <cstdint>
#include <wrl.h>

#include "CommandAllocator.h"
/// <summary>
/// コマンドキュー
/// </summary>
class CommandQueue {
	friend class CommandListManager;

public: // メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="Type">コマンドリストのタイプ</param>
	CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
	
	/// <summary>
	/// デストラクタ
	/// </summary>
	~CommandQueue();

	void Create(ID3D12Device* device);
	void Finalize();

	/// <summary>
	/// コマンドキューの取得
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); };
	
	ID3D12CommandAllocator* RequestAllocator(void);
private:
	void ExecuteCommandList(ID3D12CommandList* list);

private: // メンバ変数
	// コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;

	// コマンドリストのタイプ
	const D3D12_COMMAND_LIST_TYPE commandListType_;

	CommandAllocator allocator_;

	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
};

class CommandListManager {
public:
	CommandListManager();
	~CommandListManager();

	void Create(ID3D12Device* device);
	void Finalize();

	CommandQueue& GetGraphicsQueue(void) { return graphicsQueue_; }
	//CommandQueue& GetQueue(D3D12_COMMAND_LIST_TYPE Type = D3D12_COMMAND_LIST_TYPE_DIRECT);
	 
	ID3D12CommandQueue* GetCommandQueue() {
		return graphicsQueue_.GetCommandQueue();
	}

	void CreateNewCommandList(
		D3D12_COMMAND_LIST_TYPE type,
		ID3D12GraphicsCommandList** list,
		ID3D12CommandAllocator** allocator);

	//


private:
	ID3D12Device* device_;

	CommandQueue graphicsQueue_;

};

