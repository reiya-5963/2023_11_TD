#include "ParticleManager.h"
#include <cassert>
#include "TextureManager.h"
#include "MyD3D12Create.h"
#include "DXCManager.h"
#include "SRVManager.h"
/// <summary>
/// 静的メンバ変数の実体生成
/// </summary>
ID3D12GraphicsCommandList* ParticleManager::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> ParticleManager::sRootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> ParticleManager::sPipelineState_;
Microsoft::WRL::ComPtr<ID3D12Resource> ParticleManager::instancingResource_;
Matrix4x4 ParticleManager::sMatProjection_;
ParticleManager::ConstBufferData* ParticleManager::instancingData_ = nullptr;
D3D12_CPU_DESCRIPTOR_HANDLE ParticleManager::instancingSrvHandleCPU_;
D3D12_GPU_DESCRIPTOR_HANDLE ParticleManager::instancingSrvHandleGPU_;

//頂点バッファ
Microsoft::WRL::ComPtr<ID3D12Resource> ParticleManager::vertexBuffer_;
//頂点バッファマップ
ParticleManager::VertexPosUv* ParticleManager::vertexMap_;
//頂点バッファビュー
D3D12_VERTEX_BUFFER_VIEW ParticleManager::vertexBufferView_;
//リソース設定
D3D12_RESOURCE_DESC ParticleManager::resourceDesc_;

UINT ParticleManager::textureHandle_;


//テクスチャ始点
Vector2 ParticleManager::texBase_;
//テクスチャ幅、高さ
Vector2 ParticleManager::texSize_;


void ParticleManager::StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath) {
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

#pragma region RootSignatureの生成

#pragma region インスタンシング用のデスクリプタレンジ

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1]{};
	descriptorRangeForInstancing[0].BaseShaderRegister = 257;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

#pragma endregion

#pragma region デスクリプタレンジ

	D3D12_DESCRIPTOR_RANGE descriptorRangeSRV[1]{};
	descriptorRangeSRV[0] = MyCreate::MyInit(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

#pragma endregion

#pragma region RootParameterの設定

	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	//rootParameters[0] = MyCreate::MyInitAsConstantBufferView(0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[0] = MyCreate::MyInitAsDescriptorTable(
		_countof(descriptorRangeForInstancing), 
		descriptorRangeForInstancing, 
		D3D12_SHADER_VISIBILITY_ALL);

	rootParameters[1] = MyCreate::MyInitAsDescriptorTable(
		_countof(descriptorRangeSRV),
		descriptorRangeSRV,
		D3D12_SHADER_VISIBILITY_ALL);

#pragma endregion

#pragma region sampler

	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0] = MyCreate::MySampDescInit(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;

#pragma endregion

	//RootSignatureの生成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	descriptionRootSignature.pParameters = rootParameters;				//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	//配列の長さ
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリにする
	result = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		MyLog::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に作成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

#pragma endregion

#pragma region InputLayoutの設定

	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{	//xy座標
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	//InputLayoutの設定
	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescs;
	graphicsPipelineStateDesc.InputLayout.NumElements = _countof(inputElementDescs);

#pragma endregion

#pragma region Rasterrizerの設定

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

#pragma endregion

	//************************* TODO : パーティクル用のにする ***********************://
#pragma region 各シェーダーのコンパイル

	//各シェーダーの読み込みとコンパイル
	std::wstring vsFile = directoryPath + L"/shaders/SpriteVS.hlsl";
	vertexShaderBlob = DXCManager::GetInstance()->CompileShader(vsFile, L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	std::wstring psFile = directoryPath + L"/shaders/SpritePS.hlsl";
	pixelShaderBlob = DXCManager::GetInstance()->CompileShader(psFile, L"ps_6_0");
	assert(pixelShaderBlob != nullptr);

#pragma endregion

#pragma region PSOの生成

	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();		//RootSignature

	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };									//VertexShader

	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };										//PixelShader

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;				//RsterizerState

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//どのように画面に色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

#pragma endregion

#pragma region BlendStateの設定

#pragma region kNone

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = false;
	graphicsPipelineStateDesc.BlendState = blendDesc;
	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(result));

#pragma endregion 


#pragma endregion 

	CreateInstancingResource();

	CreateSRV();

	sMatProjection_ = R_Math::MakeOrthographicMatrix(0.0f, 0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 100.0f);

	textureHandle_ = TextureManager::Load("");
	texSize_ = {10.0f, 10.0f};

	//HRESULT result = S_FALSE;

	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

	{//vertexResourceの生成
		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(sizeof(VertexPosUv) * kVertexNum);


		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&vertexBuffer_));
		assert(SUCCEEDED(result));

		//vertexBuffer_ = CreateBufferResource(sizeof(Vector4) * kVertexNum);
		result = vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap_));
		assert(SUCCEEDED(result));
	}
	//頂点バッファへの転送
	TransferVertices();

	//頂点バッファビューの生成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = sizeof(VertexPosUv) * kVertexNum;
	vertexBufferView_.StrideInBytes = sizeof(VertexPosUv);

	//{//定数バッファの生成
	//	D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
	//	D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferData) + 0xff) & ~0xff);


	//	result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
	//		nullptr, IID_PPV_ARGS(&constBuffer_));
	//	assert(SUCCEEDED(result));


	//	//constBuffer_ = CreateBufferResource(sizeof(ConstBufferData));
	//	result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
	//	assert(SUCCEEDED(result));
	//}
}

void ParticleManager::PreDraw(ID3D12GraphicsCommandList* commandList) {
	//PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(ParticleManager::sCommandList_ == nullptr);

	//コマンドリストをセット
	sCommandList_ = commandList;

	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	//プリミティブ形状を設定
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//パイプラインステートの設定
	sCommandList_->SetPipelineState(sPipelineState_.Get());

	ConstBufferData consBuff[kParticleMax];

	for (uint32_t index = 0; index < kParticleMax; ++index) {
		consBuff[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector3 tmpScale = { 1.0f, 1.0f, 1.0f };
		Vector3 tmpRotate = { 0.0f, 0.0f, 0.0f };
		Vector3 tmpTranslate = { index * 0.1f, index * 0.1f, index * 0.1f };

		consBuff[index].mat = R_Math::MakeAffineMatrix(tmpScale, tmpRotate, tmpTranslate);
		instancingData_[index] = consBuff[index];
	}

	
	//ワールド行列の計算
	//Matrix4x4 TransWorld = R_Math::MakeAffineMatrix(worldTrans_.scale, worldTrans_.rotate, worldTrans_.translate);
	//matWorld_ = R_Math::MakeIdentity4x4();
	//matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeRotateZMatrix(rotation_));
	//matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeTranslateMatrix({ position_.x, position_.y, 0.0f }));


	//ワールド行列とビュープロジェクション行列の合成
	//matWorld_ = R_Math::Multiply(matWorld_, viewProMat);


	//それぞれ定数バッファに代入
	//constMap_->color = color_;
	//constMap_->mat = R_Math::Multiply(matWorld_, sMatProjection_);

	//頂点バッファの設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//定数バッファビューをセット
	//sCommandList_->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	sCommandList_->SetGraphicsRootDescriptorTable(1, instancingSrvHandleGPU_);

	// シェーダリソースビューをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList_, 1, textureHandle_);
	//描画コマンド
	sCommandList_->DrawInstanced(kVertexNum, kParticleMax, 0, 0);



}

void ParticleManager::PostDraw() {
	//コマンドリストを解除
	ParticleManager::sCommandList_ = nullptr;

}

void ParticleManager::CreateInstancingResource() {
	HRESULT result;
	//instancingResource_;
	//sizeof(Particle::ConstBufferData * kParticleMax);
	{//vertexResourceの生成
		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(size_t(sizeof(ConstBufferData)) * size_t(kParticleMax));


		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&instancingResource_));
		assert(SUCCEEDED(result));

		//vertexBuffer_ = CreateBufferResource(sizeof(Vector4) * kVertexNum);
		result = instancingResource_->Map(0, nullptr, reinterpret_cast<void**>(&instancingData_));
		assert(SUCCEEDED(result));
	}
	for (uint32_t index = 0; index < kParticleMax; ++index) {
		instancingData_[index].color = { 1.0f, 1.0f, 1.0f, 1.0f };
		instancingData_[index].mat = R_Math::MakeIdentity4x4();
	}
}

void ParticleManager::CreateSRV() {
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kParticleMax;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ConstBufferData);

	ID3D12DescriptorHeap* srvHeap = SRVManager::GetInstance()->GetSRVDescHeap();
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	instancingSrvHandleCPU_ = MyCreate::GetCPUDescriptorHandle(srvHeap, 257, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	instancingSrvHandleGPU_ = MyCreate::GetGPUDescriptorHandle(srvHeap, 257, device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	
	device->CreateShaderResourceView(instancingResource_.Get(), &instancingSrvDesc, instancingSrvHandleCPU_);

}

/// <summary>
/// 頂点データ転送
/// </summary>
void ParticleManager::TransferVertices() {
	//HRESULT result = S_FALSE;

		//左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	float left = (0.0f - 0.5f);
	float right = (1.0f - 0.5f);
	float top = (0.0f - 0.5f);
	float bottom = (1.0f - 0.5f);

	VertexPosUv vertices[kVertexNum];

	vertices[LB].pos = { left, bottom, 0.0f };
	vertices[LT].pos = { left, top, 0.0f };
	vertices[RB].pos = { right, bottom, 0.0f };
	vertices[RT].pos = { right, top, 0.0f };

	{
		float texLeft = texBase_.x / resourceDesc_.Width;
		float texRight = (texBase_.x + texSize_.x) / resourceDesc_.Width;
		float texTop = texBase_.y / resourceDesc_.Height;
		float texBottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;

		vertices[LB].uv = { texLeft, texBottom };
		vertices[LT].uv = { texLeft, texTop };
		vertices[RB].uv = { texRight, texBottom };
		vertices[RT].uv = { texRight, texTop };
	}

	//バッファに転送
	memcpy(vertexMap_, vertices, sizeof(vertices));
}
