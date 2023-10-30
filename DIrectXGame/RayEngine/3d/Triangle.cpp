#include "Triangle.h"
#include <cassert>
#include "TextureManager.h"
#include "MyD3D12Create.h"
#pragma comment(lib, "dxcompiler.lib")


/// <summary>
/// 静的メンバ変数の実体生成
/// </summary>
Microsoft::WRL::ComPtr<IDxcUtils> Triangle::dxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> Triangle::dxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> Triangle::includeHandler_ = nullptr;
ID3D12Device* Triangle::sDevice_ = nullptr;
ID3D12GraphicsCommandList* Triangle::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Triangle::sRootSignature_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12PipelineState> Triangle::sPipelineState_ = nullptr;
Matrix4x4 Triangle::sMatProjection_;

/// <summary>
/// シェーダーコンパイル用の関数
/// </summary>
/// <param name="filePath">ファイルの名前</param>
/// <param name="profile">ファイルのverとか</param>
/// <param name="dxcUtils">DXC</param>
/// <param name="dxcCompiler">DXC</param>
/// <param name="includeHandler">DXC</param>
/// <returns></returns>
IDxcBlob* Triangle::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile) {
#pragma region 1.hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	MyLog::Log(MyLog::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(result));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知
#pragma endregion
#pragma region 2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E", L"main",
		L"-T", profile,
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(result));
#pragma endregion
#pragma region 3.警告.エラーが出ていないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		MyLog::Log(shaderError->GetStringPointer());
		assert(false);
	}
#pragma endregion
#pragma region 4.Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//成功したらログをだす
	MyLog::Log(MyLog::ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないので開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
#pragma endregion
}

/// <summary>
///DXCの生成
/// </summary>
void Triangle::CreateDXC() {
	HRESULT result = S_FALSE;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

/// <summary>
/// 静的初期化	
/// </summary>
/// <param name="device">デバイス</param>
/// <param name="directoryPath">shadersファイルまでのパス</param>
void Triangle::StaticInitialize(ID3D12Device* device, int windowWidth, int windowHeight, const std::wstring& directoryPath) {
	assert(device);
	sDevice_ = device;
	//DXCの生成
	CreateDXC();

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
	result = sDevice_->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
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
#pragma region BlendStateの設定
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
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
	vertexShaderBlob = CompileShader(vsFile, L"vs_6_0");
	assert(vertexShaderBlob != nullptr);
	std::wstring psFile = directoryPath + L"/shaders/SpritePS.hlsl";
	pixelShaderBlob = CompileShader(psFile, L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
#pragma endregion
#pragma region PSOの生成
	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();		//RootSignature
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };									//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };										//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;						//BlendState
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

	//実際に生成
	result = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&sPipelineState_));
	assert(SUCCEEDED(result));
#pragma endregion
	sMatProjection_ = R_Math::MakeOrthographicMatrix(0.0f, 0.0f, (float)windowWidth, (float)windowHeight, 0.0f, 100.0f);
}

/// <summary>
/// 描画前処理
/// </summary>
/// <param name="commandList">描画コマンドリスト</param>
void Triangle::PreDraw(ID3D12GraphicsCommandList* commandList) {
	//PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Triangle::sCommandList_ == nullptr);

	//コマンドリストをセット
	sCommandList_ = commandList;

	//ルートシグネチャの設定
	sCommandList_->SetGraphicsRootSignature(sRootSignature_.Get());
	//パイプラインステートの設定
	sCommandList_->SetPipelineState(sPipelineState_.Get());
	//プリミティブ形状を設定
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

/// <summary>
/// 描画後処理
/// </summary>
void Triangle::PostDraw() {
	//コマンドリストを解除
	Triangle::sCommandList_ = nullptr;
}

/// <summary>
/// スプライト生成
/// </summary>
/// <param name="position">座標</param>
/// <param name="rotate">回転</param>
/// <param name="scale">大きさ</param>
/// <returns>生成されたスプライト</returns>
Triangle* Triangle::Create(uint32_t tectureHandle, Vector3 position,
	Vector4 color, Vector2 anchorpoint) {
	Vector2 size = { 100.0f, 100.0f };

	{
		const D3D12_RESOURCE_DESC& resourceDesc =
			TextureManager::GetInstance()->GetResourceDesc(tectureHandle);
		size = { (float)resourceDesc.Width, (float)resourceDesc.Height };
	}


	//Triangleのインスタンスを生成
	Triangle* triangle = new Triangle(tectureHandle, position, size, color, anchorpoint);
	if (triangle == nullptr) {
		return nullptr;
	}

	//初期化
	if (!triangle->Initialize()) {
		delete triangle;
		assert(0);
		return nullptr;
	}
	return triangle;
}

/// <summary>
/// コンストラクタ
/// </summary>
Triangle::Triangle() {}

/// <summary>
/// コンストラクタ
/// </summary>
/// <param name="position">座標</param>
/// <param name="rotate">回転</param>
/// <param name="scale">大きさ</param>
Triangle::Triangle(uint32_t textureHandle, Vector3 position, Vector2 size,
	Vector4 color, Vector2 anchorPoint) {
	//worldTrans_.rotate = rotate;
	worldTrans_.scale.x = size.x;
	worldTrans_.scale.y = size.y;
	worldTrans_.translate = position;
	anchorPoint_ = anchorPoint;
	matWorld_ = R_Math::MakeIdentity4x4();
	color_ = color;
	textureHandle_ = textureHandle;
	texSize_ = size;
}

/// <summary>
/// 初期化
/// </summary>
/// <returns>できたかどうか</returns>
bool Triangle::Initialize() {
	assert(sDevice_);
	HRESULT result = S_FALSE;

	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);

	{//vertexResourceの生成
		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(sizeof(VertexPosUv) * kVertexNum);


		result = sDevice_->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
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


		result = sDevice_->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
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
void Triangle::Draw(Matrix4x4 viewProMatrix) {
	//ワールド行列の計算
	Matrix4x4 worldMatrix = R_Math::MakeAffineMatrix(worldTrans_.scale, worldTrans_.rotate, worldTrans_.translate);
	matWorld_ = R_Math::Multiply(worldMatrix, viewProMatrix);

	//それぞれ定数バッファに代入
	constMap_->color = color_;
	constMap_->mat = matWorld_;

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
void Triangle::TransferVertices() {
	//左下、上、右下
	enum { LB, T, RB };

	float left = (0.0f - anchorPoint_.x) * worldTrans_.scale.x;
	float right = (1.0f - anchorPoint_.x) * worldTrans_.scale.x;
	float top = (0.0f - anchorPoint_.y) * worldTrans_.scale.y;
	float bottom = (1.0f - anchorPoint_.y) * worldTrans_.scale.y;

	top = -top;
	bottom = -bottom;

	VertexPosUv vertices[kVertexNum];

	vertices[LB].pos = { left, bottom, 0.0f, 1.0f };
	vertices[T].pos = { (right - (right / 2)), top, 0.0f , 1.0f };
	vertices[RB].pos = { right, bottom, 0.0f , 1.0f };

	vertices[LB].uv = { 0.0f, 1.0f };
	vertices[T].uv = { 0.5f, 0.0f };
	vertices[RB].uv = { 1.0f, 1.0f };
	
	//バッファに転送
	memcpy(vertexMap_, vertices, sizeof(vertices));
}

void Triangle::SetSize(const Vector2& size) {
	size_ = size;
	TransferVertices();
}

void Triangle::SetTranslate(Vector3 translate) {
	worldTrans_.translate = translate;
	TransferVertices();
}

void Triangle::SetRotate(Vector3 rotate) {
	worldTrans_.rotate = rotate;
	TransferVertices();
}

void Triangle::SetScale(Vector3 scale) {
	worldTrans_.scale = scale;
	TransferVertices();
}

void Triangle::SetColor(Vector4 color) {
	color_ = color;
}

void Triangle::SetTextureRect(const Vector2& texBase, const Vector2 texSize) {
	texBase_ = texBase;
	texSize_ = texSize;
	TransferVertices();

}

void Triangle::SetTextureHandle(uint32_t textureHandle) {
	textureHandle_ = textureHandle;
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
}