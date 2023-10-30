#include "DXCommon.h"
#include "MyD3D12Create.h"
#include "viewProjection.h"
#include "WinApp.h"

#include <cassert>

void ViewProjection::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void ViewProjection::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferDataViewProjection) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void ViewProjection::Map() {
	HRESULT result;
	// 定数バッファとのデータリンク
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

}

void ViewProjection::UpdateMatrix() {
	UpdateViewMatrix();
	UpdateProjectionMatrix();
	TransferMatrix();
}

void ViewProjection::TransferMatrix() {
	constMap->view = matView;
	constMap->projection = matProjection;
	
	Matrix4x4 matTranslate = R_Math::MakeTranslateMatrix(translation_);
	
	constMap->cameraPos.x = matTranslate.m[3][0];
	constMap->cameraPos.y = matTranslate.m[3][1];
	constMap->cameraPos.z = matTranslate.m[3][2];
}

void ViewProjection::UpdateViewMatrix() {
	// ビュー行列の生成
	matView = R_Math::Inverse(R_Math::MakeAffineMatrix(scale_, rotation_, translation_));
}

void ViewProjection::UpdateProjectionMatrix() {
	// 透視投影による射影行列の生成
	matProjection = R_Math::MakePerspectiveFovMatrix(fovAngleY, aspectRatio, nearZ, farZ);
}
