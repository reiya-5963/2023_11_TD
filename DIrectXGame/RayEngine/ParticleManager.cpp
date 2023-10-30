//#include "ParticleManager.h"
//#include "Particle.h"
//#include "R_Math.h"
//#include <cassert>
//#include "TextureManager.h"
//#include "MyD3D12Create.h"
////#pragma comment(lib, "dxcompiler.lib")
//#include "DXCManager.h"
//
///// <summary>
///// 静的メンバ変数の実体生成
///// </summary>
//ID3D12GraphicsCommandList* ParticleManager::sCommandList_ = nullptr;
//Microsoft::WRL::ComPtr<ID3D12RootSignature> ParticleManager::sRootSignature_ = nullptr;
//std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(ParticleManager::BlendMode::BlendModeCount)> ParticleManager::sPipelineStates_;
//Matrix4x4 ParticleManager::sMatProjection_;
//std::list<Particle*> ParticleManager::particles_;
//UINT ParticleManager::textureHandle_;
//
///// <summary>
///// 静的初期化	
///// </summary>
///// <param name="device">デバイス</param>
///// <param name="directoryPath">shadersファイルまでのパス</param>
//void ParticleManager::StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath) {
//	//下準備
//	HRESULT result = S_FALSE;
//	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
//	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
//	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
//	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;
//	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
//
//#pragma region RootSignatureの生成
//
//#pragma region デスクリプタレンジ
//	D3D12_DESCRIPTOR_RANGE descriptorRangeSRV[1]{};
//	descriptorRangeSRV[0] = MyCreate::MyInit(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
//
//	D3D12_DESCRIPTOR_RANGE descriptorRangeforInstancing[1]{};
//	descriptorRangeforInstancing[0].BaseShaderRegister = 0;
//	descriptorRangeforInstancing[0].NumDescriptors = 1;
//	descriptorRangeforInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
//	descriptorRangeforInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
//
//#pragma endregion
//#pragma region RootParameterの設定
//	D3D12_ROOT_PARAMETER rootParameters[2] = {};
//	// Constant Buffer View
//	rootParameters[0] = MyCreate::MyInitAsDescriptorTable(_countof(descriptorRangeforInstancing), descriptorRangeforInstancing, D3D12_SHADER_VISIBILITY_ALL);
//	// Shader Resource View
//	rootParameters[1] = MyCreate::MyInitAsDescriptorTable(1, descriptorRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
//#pragma endregion
//#pragma region sampler
//	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
//	staticSamplers[0] = MyCreate::MySampDescInit(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
//	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
//	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
//	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
//
//#pragma endregion
//
//	//RootSignatureの生成
//	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
//	descriptionRootSignature.Flags =
//		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//	descriptionRootSignature.pParameters = rootParameters;				//ルートパラメータ配列へのポインタ
//	descriptionRootSignature.NumParameters = _countof(rootParameters);	//配列の長さ
//	descriptionRootSignature.pStaticSamplers = staticSamplers;
//	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
//
//	//シリアライズしてバイナリにする
//	result = D3D12SerializeRootSignature(&descriptionRootSignature,
//		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
//	if (FAILED(result)) {
//		MyLog::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
//		assert(false);
//	}
//	//バイナリを元に作成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
//		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&sRootSignature_));
//	assert(SUCCEEDED(result));
//#pragma endregion
//#pragma region InputLayoutの設定
//	//頂点レイアウト
//	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
//		{	//xy座標
//		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
//		{
//		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT,
//		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
//	};
//	//InputLayoutの設定
//	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescs;
//	graphicsPipelineStateDesc.InputLayout.NumElements = _countof(inputElementDescs);
//#pragma endregion
//#pragma region Rasterrizerの設定
//	//RasterizerStateの設定
//	D3D12_RASTERIZER_DESC rasterizerDesc{};
//	//裏面(時計回り)を表示しない
//	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
//	//三角形の中を塗りつぶす
//	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
//#pragma endregion
//#pragma region 各シェーダーのコンパイル
//	//各シェーダーの読み込みとコンパイル
//	std::wstring vsFile = directoryPath + L"/shaders/ParticleVS.hlsl";
//	vertexShaderBlob = DXCManager::GetInstance()->CompileShader(vsFile, L"vs_6_0");
//	assert(vertexShaderBlob != nullptr);
//	std::wstring psFile = directoryPath + L"/shaders/ParticlePS.hlsl";
//	pixelShaderBlob = DXCManager::GetInstance()->CompileShader(psFile, L"ps_6_0");
//	assert(pixelShaderBlob != nullptr);
//#pragma endregion
//#pragma region PSOの生成
//	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();		//RootSignature
//	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
//	vertexShaderBlob->GetBufferSize() };									//VertexShader
//	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
//	pixelShaderBlob->GetBufferSize() };										//PixelShader
//	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;				//RsterizerState
//	//書き込むRTVの情報
//	graphicsPipelineStateDesc.NumRenderTargets = 1;
//	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
//	//利用するトポロジ(形状)のタイプ。三角形
//	graphicsPipelineStateDesc.PrimitiveTopologyType =
//		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//	//どのように画面に色を打ち込むかの設定(気にしなくていい)
//	graphicsPipelineStateDesc.SampleDesc.Count = 1;
//	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
//
//	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
//	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
//
//
//#pragma endregion
//
//#pragma region BlendStateの設定
//
//#pragma region kNone
//	//BlendStateの設定
//	D3D12_BLEND_DESC blendDesc{};
//	//全ての色要素を書き込む
//	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
//	blendDesc.RenderTarget[0].BlendEnable = false;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNone)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//#pragma region kNormal	
//	blendDesc.RenderTarget[0].BlendEnable = true;
//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
//	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
//	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNone)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//#pragma region kAdd
//
//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kAdd)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//#pragma region kSubtract
//
//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kSubtract)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//#pragma region kMultiply
//
//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kMultiply)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//#pragma region kScreen
//
//	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
//	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
//	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
//	graphicsPipelineStateDesc.BlendState = blendDesc;
//
//	//実際に生成
//	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
//		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kScreen)]));
//	assert(SUCCEEDED(result));
//#pragma endregion 
//
//
//#pragma endregion 
//
//	sMatProjection_ = R_Math::MakeOrthographicMatrix(0.0f, 0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 100.0f);
//
//
//}
//
//void ParticleManager::PreDraw(ID3D12GraphicsCommandList* commandList)
//{	//PreDrawとPostDrawがペアで呼ばれていなければエラー
//	assert(ParticleManager::sCommandList_ == nullptr);
//
//
//
//	//コマンドリストをセット
//	sCommandList_ = commandList;
//
//	//ルートシグネチャの設定
//	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
//	//プリミティブ形状を設定
//	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
//
//}
//
//void ParticleManager::PostDraw()
//{	//コマンドリストを解除
//	ParticleManager::sCommandList_ = nullptr;
//
//}
//
//void ParticleManager::Draw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) {
//	PreDraw(commandList);
//
//	//パイプラインステートの設定
//	sCommandList_->SetPipelineState(sPipelineStates_[size_t(blendMode)].Get());
//
//
//	for (Particle* particle : particles_) {
//		particle->Draw(commandList);
//	}
//	
//	// シェーダリソースビューをセット
//	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList_, 1, textureHandle_);
//	//描画コマンド
//	sCommandList_->DrawInstanced(kVertexNum * UINT(particles_.size()), UINT(particles_.size()), 0, 0);
//
//	PostDraw();
//}
//
//void ParticleManager::AddParticle(Particle* particle)
//{
//	particles_.emplace_back(particle);
//}
//
//void ParticleManager::SetTextureHandle(uint32_t textureHandle) {
//	textureHandle_ = textureHandle;
//	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
//}
//
//void ParticleManager::Initialize() {
//	{//vertexResourceの生成
//		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
//		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(sizeof(VertexPosUv) * kVertexNum);
//
//
//		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
//			nullptr, IID_PPV_ARGS(&vertexBuffer_));
//		assert(SUCCEEDED(result));
//
//		//vertexBuffer_ = CreateBufferResource(sizeof(Vector4) * kVertexNum);
//		result = vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertexMap_));
//		assert(SUCCEEDED(result));
//	}
//	//頂点バッファへの転送
//	TransferVertices();
//
//	//頂点バッファビューの生成
//	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
//	vertexBufferView_.SizeInBytes = sizeof(VertexPosUv) * kVertexNum;
//	vertexBufferView_.StrideInBytes = sizeof(VertexPosUv);
//
//	{//定数バッファの生成
//		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
//		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferData) + 0xff) & ~0xff);
//
//
//		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
//			nullptr, IID_PPV_ARGS(&constBuffer_));
//		assert(SUCCEEDED(result));
//
//
//		//constBuffer_ = CreateBufferResource(sizeof(ConstBufferData));
//		result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
//		assert(SUCCEEDED(result));
//	}
//
//}
