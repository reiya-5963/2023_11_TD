#include "Sprite.h"
#include <cassert>
#include "TextureManager.h"
#include "MyD3D12Create.h"
//#pragma comment(lib, "dxcompiler.lib")
#include "DXCManager.h"

/// <summary>
/// 静的メンバ変数の実体生成
/// </summary>
ID3D12GraphicsCommandList* Sprite::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Sprite::sRootSignature_ = nullptr;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(Sprite::BlendMode::BlendModeCount)> Sprite::sPipelineStates_;
Matrix4x4 Sprite::sMatProjection_;

/// <summary>
/// 静的初期化	
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="directoryPath">shadersファイルまでのパス</param>
void Sprite::StaticInitialize(int windowWidth, int windowHeight, const std::wstring& directoryPath) {
	//下準備
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

#pragma region RootSignatureの生成

#pragma region デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE descriptorRangeSRV[1]{};
	descriptorRangeSRV[0] = MyCreate::MyInit(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
#pragma endregion
#pragma region RootParameterの設定
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0] = MyCreate::MyInitAsConstantBufferView(0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1] = MyCreate::MyInitAsDescriptorTable(1, descriptorRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
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
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNone)]));
	assert(SUCCEEDED(result));
#pragma endregion 

#pragma region kNormal	
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNone)]));
	assert(SUCCEEDED(result));
#pragma endregion 

#pragma region kAdd

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kAdd)]));
	assert(SUCCEEDED(result));
#pragma endregion 

#pragma region kSubtract

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kSubtract)]));
	assert(SUCCEEDED(result));
#pragma endregion 

#pragma region kMultiply

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kMultiply)]));
	assert(SUCCEEDED(result));
#pragma endregion 

#pragma region kScreen

	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	graphicsPipelineStateDesc.BlendState = blendDesc;

	//実際に生成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kScreen)]));
	assert(SUCCEEDED(result));
#pragma endregion 


#pragma endregion 

	sMatProjection_ = R_Math::MakeOrthographicMatrix(0.0f, 0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 100.0f);
}

/// <summary>
/// 描画前処理
/// </summary>
/// <param name="commandList">描画コマンドリスト</param>
void Sprite::PreDraw(ID3D12GraphicsCommandList* commandList) {
	//PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Sprite::sCommandList_ == nullptr);

	//コマンドリストをセット
	sCommandList_ = commandList;

	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	//プリミティブ形状を設定
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

/// <summary>
/// 描画後処理
/// </summary>
void Sprite::PostDraw() {
	//コマンドリストを解除
	Sprite::sCommandList_ = nullptr;
}

/// <summary>
/// スプライト生成
/// </summary>
/// <param name="position">座標</param>
/// <param name="rotate">回転</param>
/// <param name="scale">大きさ</param>
/// <returns>生成されたスプライト</returns>
Sprite* Sprite::Create(uint32_t tectureHandle, Vector2 position, float rotate,
	Vector4 color, Vector2 anchorpoint, bool isFlipX,
	bool isFlipY) {
	// 仮サイズ
	Vector2 size = { 100.0f, 100.0f };

	{
		// テクスチャの情報を取得
		const D3D12_RESOURCE_DESC& resourceDesc =
			TextureManager::GetInstance()->GetResourceDesc(tectureHandle);
		// サイズをテクスチャのサイズに設定
		size = { (float)resourceDesc.Width, (float)resourceDesc.Height };
	}


	//Spriteのインスタンスを生成
	Sprite* sprite = new Sprite(tectureHandle, position, rotate, size, color, anchorpoint, isFlipX, isFlipY);
	if (sprite == nullptr) {
		return nullptr;
	}


	//初期化
	if (!sprite->Initialize()) {
		delete sprite;
		assert(0);
		return nullptr;
	}
	return sprite;
}



/// <summary>
/// コンストラクタ
/// </summary>
Sprite::Sprite() {}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="position">座標</param>
/// <param name="rotate">回転</param>
/// <param name="scale">大きさ</param>
Sprite::Sprite(uint32_t textureHandle, Vector2 position, float rotate, Vector2 size,
	Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY) {
	position_ = position;
	rotation_ = rotate;
	size_ = size;
	anchorPoint_ = anchorPoint;
	matWorld_ = R_Math::MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
	isFlipX_ = isFlipX;
	isFlipY_ = isFlipY;
	texSize_ = size;
}

/// <summary>
/// 初期化
/// </summary>
/// <returns>できたかどうか</returns>
bool Sprite::Initialize() {
	HRESULT result = S_FALSE;

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

	{//定数バッファの生成
		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc((sizeof(ConstBufferData) + 0xff) & ~0xff);


		result = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr, IID_PPV_ARGS(&constBuffer_));
		assert(SUCCEEDED(result));


		//constBuffer_ = CreateBufferResource(sizeof(ConstBufferData));
		result = constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap_));
		assert(SUCCEEDED(result));
	}

	return true;
}


/// <summary>
/// 描画
/// </summary>
/// <param name="viewProMat">ViewProjectionMatrix</param>
void Sprite::Draw(BlendMode blendMode) {
	//パイプラインステートの設定
	sCommandList_->SetPipelineState(sPipelineStates_[size_t(blendMode)].Get());


	//ワールド行列の計算
	//Matrix4x4 TransWorld = R_Math::MakeAffineMatrix(worldTrans_.scale, worldTrans_.rotate, worldTrans_.translate);
	matWorld_ = R_Math::MakeIdentity4x4();
	matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeRotateZMatrix(rotation_));
	matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeTranslateMatrix({ position_.x, position_.y, 0.0f }));


	//ワールド行列とビュープロジェクション行列の合成
	//matWorld_ = R_Math::Multiply(matWorld_, viewProMat);


	//それぞれ定数バッファに代入
	constMap_->color = color_;
	constMap_->mat = R_Math::Multiply(matWorld_, sMatProjection_);

	//頂点バッファの設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//定数バッファビューをセット
	sCommandList_->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
	// シェーダリソースビューをセット
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(sCommandList_, 1, textureHandle_);
	//描画コマンド
	sCommandList_->DrawInstanced(kVertexNum, 1, 0, 0);
}

/// <summary>
/// 頂点データ転送
/// </summary>
void Sprite::TransferVertices() {
	//HRESULT result = S_FALSE;

		//左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;

	if (isFlipX_) {
		left = -left;
		right = -right;
	}

	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}


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

void Sprite::SetSize(const Vector2& size) {
	size_ = size;
	TransferVertices();
}

void Sprite::SetPosition(const Vector2& position) {
	position_ = position;
	TransferVertices();
}

void Sprite::SetRotate(const float& rotate) {
	rotation_ = rotate;
	TransferVertices();
}

void Sprite::SetColor(const Vector4& color) {
	color_ = color;
}

void Sprite::SetTextureRect(const Vector2& texBase, const Vector2 texSize) {
	texBase_ = texBase;
	texSize_ = texSize;
	TransferVertices();
}

void Sprite::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
}