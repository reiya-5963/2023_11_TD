#include "ImGuiManager.h"
#include "MyD3D12Create.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize(WinApp* winApp, DirectXCommon* dxCommon) {
	//HRESULT result;

	dxCommon_ = dxCommon;
	//MyCreate::CreateDescriptorHeap(dxCommon->GetDevice(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	/*D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 1;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	result = dxCommon->GetDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap_));
	assert(SUCCEEDED(result));*/
	//srvHeap_->SetName(L"ImGui_DescriptorHeap");

	srvHeap_ = SRVManager::GetInstance()->GetSRVDescHeap();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ID3D12Device* device = dxCommon_->GetDevice();

	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle;
	srvCPUHandle = MyCreate::GetCPUDescriptorHandle(srvHeap_, 256, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUHandle;
	srvGPUHandle = MyCreate::GetGPUDescriptorHandle(srvHeap_, 256, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));

	ImGui_ImplWin32_Init(winApp->GetHwnd());
	ImGui_ImplDX12_Init(
		dxCommon_->GetDevice(), static_cast<int>(dxCommon_->GetBackBufferCount()),
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap_,
		srvCPUHandle,
		srvGPUHandle);
	
	ImGui::CreateContext();
	auto& io = ImGui::GetIO();

	// 
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

}

void ImGuiManager::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//srvHeap_.Reset();
}


void ImGuiManager::Begin() {
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End() {
	ImGui::Render();
}

void ImGuiManager::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap_ };
	commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);

}
