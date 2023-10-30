#include "DXCommon.h"
#include "WorldTransform.h"
#include "MyD3D12Create.h"
#include <cassert>

void WorldTransform::Initialize() {
	CreateConstBuffer();
	Map();
	UpdateMatrix();
}

void WorldTransform::CreateConstBuffer() {
	HRESULT result;

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferDataWorldTransform) + 0xff) & ~0xff);

	// 定数バッファの生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));
}

void WorldTransform::Map() {
	HRESULT result;
	// 定数バッファとのデータリンク
	result = constBuff_->Map(0, nullptr, (void**)&constMap);
	assert(SUCCEEDED(result));

}


void WorldTransform::TransferMatrix() {
	// 定数バッファ
	constMap->matWorld = matWorld_;
}

void WorldTransform::UpdateMatrix() {
	matWorld_ = R_Math::MakeAffineMatrix(scale_, rotation_, translation_);

	if (parent_) {
		matWorld_ = R_Math::Multiply(matWorld_, parent_->matWorld_);
	}
	TransferMatrix();
}

