#include "LightGroup.h"
#include "DXCommon.h"
#include "MyD3D12Create.h"
#include <cassert>

LightGroup* LightGroup::Create() {
	// 3Dオブジェクトのインスタンスを生成
	LightGroup* instance = new LightGroup();

	// 初期化
	instance->Initialize();
	
	return instance;
}

void LightGroup::Initialize() {
	DefaultLightSetting();

	// ヒーププロパティ
	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	// リソース設定
	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferData) + 0xff) & ~0xff);

	HRESULT result;
	// 定数バッファの生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProps,
		D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&constBuff_));
	assert(SUCCEEDED(result));

	// 定数バッファとのデータリンク
	result = constBuff_->Map(0, nullptr, (void**)&constMap_);
	assert(SUCCEEDED(result));

	// 定数バッファへデータ転送
	TransferConstBuffer();
}

void LightGroup::Update() {
	// 値の更新があった時だけ定数バッファに転送する
	if (dirty_) {
		TransferConstBuffer();
		dirty_ = false;
	}
}

void LightGroup::Draw(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
	// 定数バッファビューをセット
	commandList->SetGraphicsRootConstantBufferView(
		rootParameterIndex, constBuff_->GetGPUVirtualAddress());
}

void LightGroup::TransferConstBuffer() {
	// 環境光
	constMap_->ambientColor = ambientColor_;
	// 平行光源
	for (int i = 0; i < kDirectLightNum; i++) {
		// ライトが有効なら設定を転送
		if (directLights_[i].IsActive()) {
			constMap_->directLights[i].active = 1;
			constMap_->directLights[i].lightv = { -directLights_[i].GetLightDir().x,-directLights_[i].GetLightDir().y, -directLights_[i].GetLightDir().z };
			constMap_->directLights[i].lightcolor = directLights_[i].GetLightColor();
		}
		// ライトが無効ならライト色を0に
		else {
			constMap_->directLights[i].active = 0;
		}
	}

}

void LightGroup::DefaultLightSetting() {
	directLights_[0].SetActive(true);
	directLights_[0].SetLightColor({ 1.0f, 1.0f, 1.0f });
	directLights_[0].SetLightDir({ 0.0f, -1.0f, 0.0f });

	directLights_[1].SetActive(true);
	directLights_[1].SetLightColor({ 1.0f, 1.0f, 1.0f });
	directLights_[1].SetLightDir({ +0.5f, -0.1f, +0.2f });

	directLights_[2].SetActive(true);
	directLights_[2].SetLightColor({ 1.0f, 1.0f, 1.0f });
	directLights_[2].SetLightDir({ -0.5f, +0.1f, -0.2f });

}

void LightGroup::SetAmbientColor(const Vector3& color) {
	ambientColor_ = color;
	dirty_ = true;
}

void LightGroup::SetDirectLightActive(int index, bool active) {
	assert(0 <= index && index < kDirectLightNum);
	directLights_[index].SetActive(active);
}

void LightGroup::SetDirectLightDir(int index, const Vector3& lightDir) {
	assert(0 <= index && index < kDirectLightNum);
	directLights_[index].SetLightDir(lightDir);
	dirty_ = true;
}

void LightGroup::SetDirectLightColor(int index, const Vector3& lightColor) {
	assert(0 <= index && index < kDirectLightNum);
	directLights_[index].SetLightColor(lightColor);
	dirty_ = true;
}
