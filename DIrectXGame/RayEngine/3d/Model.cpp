#include "DXCommon.h"
#include "Model.h"
#include "DXCManager.h"
#include "MyD3D12Create.h"
#include "MyLog.h"
#include <cassert>
#include <fstream>
#include <sstream>


/// 静的な変数
const std::string Model::kBaseDirectory = "Resources/";
const std::string Model::kDefaultModelName = "cube";
UINT Model::sDescreptorHandleIncrementSize_ = 0;
ID3D12GraphicsCommandList* Model::sCommandList_ = nullptr;
Microsoft::WRL::ComPtr<ID3D12RootSignature> Model::sRootSignature_;
std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, size_t(Model::BlendMode::kCountBlendMode)> Model::sPipelineStates_;
std::unique_ptr<LightGroup> Model::lightGroup_;
////


void Model::StaticInitialize() {
	// パイプライン初期化
	InitializeGraphicsPipeline();

	// ライト生成
	lightGroup_.reset(LightGroup::Create());
}

void Model::InitializeGraphicsPipeline() {
	HRESULT result = S_FALSE;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob; //
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob; //
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob; //
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};

#pragma region 各シェーダーのコンパイル
	vertexShaderBlob = DXCManager::GetInstance()->CompileShader(
		L"Resources/shaders/ObjVS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	pixelShaderBlob = DXCManager::GetInstance()->CompileShader(
		L"Resources/shaders/ObjPS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);
#pragma endregion
#pragma region InputLayoutの設定
	//頂点レイアウト
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
		{	//xy座標
		"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{
		"NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,	0, D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
#pragma endregion
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
		vertexShaderBlob->GetBufferSize() };									//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
		pixelShaderBlob->GetBufferSize() };										//PixelShader

	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

#pragma region Rasterrizerの設定
	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;				//RsterizerState
#pragma endregion
	graphicsPipelineStateDesc.DepthStencilState.DepthEnable = true;
	graphicsPipelineStateDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	graphicsPipelineStateDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//InputLayoutの設定
	graphicsPipelineStateDesc.InputLayout.pInputElementDescs = inputElementDescs;
	graphicsPipelineStateDesc.InputLayout.NumElements = _countof(inputElementDescs);

	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//どのように画面に色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;

#pragma region RootSignatureの生成
#pragma region デスクリプタレンジ
	D3D12_DESCRIPTOR_RANGE descriptorRangeSRV[1]{};
	descriptorRangeSRV[0] = MyCreate::MyInit(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
#pragma endregion
#pragma region RootParameterの設定
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0] = MyCreate::MyInitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[1] = MyCreate::MyInitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2] = MyCreate::MyInitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[3] = MyCreate::MyInitAsDescriptorTable(1, descriptorRangeSRV, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[4] = MyCreate::MyInitAsConstantBufferView(3, 0, D3D12_SHADER_VISIBILITY_ALL);
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
		D3D_ROOT_SIGNATURE_VERSION_1_0, &signatureBlob, &errorBlob);
	if (FAILED(result)) {
		MyLog::Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に作成
	result = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&sRootSignature_));
	assert(SUCCEEDED(result));

	graphicsPipelineStateDesc.pRootSignature = sRootSignature_.Get();		//RootSignature
#pragma endregion


#pragma region BlendStateの設定
#pragma region kNone
	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//全ての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = false;
	graphicsPipelineStateDesc.BlendState = blendDesc;						//BlendState
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
		IID_PPV_ARGS(&sPipelineStates_[size_t(BlendMode::kNormal)]));
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


#pragma endregion
}

Model* Model::Create() {
	// メモリ確保
	Model* instance = new Model;
	instance->Initialize(kDefaultModelName);
	
	return instance;
}

Model* Model::CreateFlomObj(const std::string& modelname) {
	// メモリ確保
	Model* instance = new Model;
	instance->Initialize(modelname);

	return instance;
}

void Model::PreDraw(ID3D12GraphicsCommandList* commandList, BlendMode blendMode) {
	// PreDrawとPostDrawが両方呼ばれていなければエラー
	assert(Model::sCommandList_ == nullptr);

	// コマンドリストをセット
	sCommandList_ = commandList;

	// パイプラインステートの設定
	commandList->SetPipelineState(sPipelineStates_[size_t(blendMode)].Get());
	// ルートシグネチャの設定
	commandList->SetGraphicsRootSignature(sRootSignature_.Get());
	// プリミティブ形状を設定
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Model::PostDraw() {
	// コマンドリストの解除
	sCommandList_ = nullptr;
}

Model::~Model() {
	for (auto m : meshes_) {
		delete m;
	}
	meshes_.clear();
	for (auto m : materials_) {
		delete m.second;
	}
	materials_.clear();
}

void Model::Initialize(const std::string& modelname) {
	// モデル読み込み
	LoadModel(modelname);

	// メッシュのマテリアルチェック
	for (auto& m : meshes_) {
		// マテリアルの割り当てがない
		if (m->GetMaterial() == nullptr) {
			if (defaultMaterial_ == nullptr) {
				// デフォルトマテリアルを生成
				defaultMaterial_ = Material::Create();
				defaultMaterial_->name_ = "no material";
				materials_.emplace(defaultMaterial_->name_, defaultMaterial_);
			}
			// デフォルトマテリアルをセット
			m->SetMaterial(defaultMaterial_);
		}
	}

	// メッシュのバッファ生成
	for (auto& m : meshes_) {
		m->CreateBuffers();
	}

	// マテリアルの数値を定数バッファに反映
	for (auto& m : materials_) {
		m.second->Update();
	}

	// テクスチャの読み込み
	LoadTextures();
}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection) {

	// ライトの描画
	lightGroup_->Draw(sCommandList_, static_cast<UINT>(RootParameter::kLight));

	// CBVをセット (ワールド行列)
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RootParameter::kWorldTransform),
		worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット (ビュープロジェクション行列)
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RootParameter::kViewProjection),
		viewProjection.constBuff_->GetGPUVirtualAddress());
	
	// 全メッシュを描画
	for (auto& mesh : meshes_) {
		mesh->Draw(sCommandList_, (UINT)RootParameter::kMaterial, (UINT)RootParameter::kTexture);
	}

}

void Model::Draw(const WorldTransform& worldTransform, const ViewProjection& viewProjection, uint32_t textireHandle) {

	// ライトの描画
	lightGroup_->Draw(sCommandList_, static_cast<UINT>(RootParameter::kLight));


	// CBVをセット (ワールド行列)
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RootParameter::kWorldTransform),
		worldTransform.constBuff_->GetGPUVirtualAddress());

	// CBVをセット (ビュープロジェクション行列)
	sCommandList_->SetGraphicsRootConstantBufferView(
		static_cast<UINT>(RootParameter::kViewProjection),
		viewProjection.constBuff_->GetGPUVirtualAddress());

	// 全メッシュを描画
	for (auto& mesh : meshes_) {
		mesh->Draw(sCommandList_, (UINT)RootParameter::kMaterial, (UINT)RootParameter::kTexture,
				   textireHandle);
	}

}

void Model::LoadModel(const std::string& modelname) {
	const string filename = modelname + ".obj";
	const string directoryPath = kBaseDirectory + modelname + "/";

	// ファイルストリーム
	std::ifstream file;
	// .objファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	name_ = modelname;

	// メッシュ生成
	meshes_.emplace_back(new Mesh);
	Mesh* mesh = meshes_.back();
	int indexCountTex = 0;
	//int indexCountNoTex = 0;

	vector<Vector3> positions;	// 頂点座標
	vector<Vector3> normals;	// 法線ベクトル
	vector<Vector2> texcoords;	// テクスチャUV
	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {
		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// マテリアル
		if (key == "mtllib") {
			// マテリアルのファイル名読み込み
			string materialFilename;
			line_stream >> materialFilename;
			// マテリアル読み込み
			LoadMaterial(directoryPath, materialFilename);
		}
		// 先頭文字列がgならグループの開始
		if (key == "g") {
			// カレントメッシュの情報がそろっているなら
			if (mesh->GetName().size() > 0 && mesh->GetVertexCount() > 0) {
				// 頂点法線の平均によるエッジの平滑化
				/*if (smoothing) {
					mesh->CalculateSmoothVertexNormals();
				}*/
				// 次のメッシュ生成
				meshes_.emplace_back(new Mesh);
				mesh = meshes_.back();
				indexCountTex = 0;
			}

			// グループ名読み込み
			string groupName;
			line_stream >> groupName;

			// メッシュに名前をセット
			mesh->SetName(groupName);
		}
		// 先頭文字列がvなら頂点座標
		if (key == "v") {
			// x, y, z座標読み込み
			Vector3 position{};
			line_stream >> position.x >> position.y >> position.z;
			positions.emplace_back(position);
		}
		// 先頭文字列がvtならテクスチャ
		if (key == "vt") {
			// u, v成分読み込み
			Vector2 texcoord{};
			line_stream >> texcoord.x >> texcoord.y;
			// vのみ反転
			texcoord.y = 1.0f - texcoord.y;
			texcoords.emplace_back(texcoord);
		}
		// 先頭文字列がvnなら法線ベクトル
		if (key == "vn") {
			// x, y, z成分読み込み
			Vector3 normal{};
			line_stream >> normal.x >> normal.y >> normal.z;
			normals.emplace_back(normal);
		}
		// 先頭文字列がusemtlならマテリアルを割り当てる
		if (key == "usemtl") {
			if (mesh->GetMaterial() == nullptr) {
				// マテリアルの名前読み込み
				string materialName;
				line_stream >> materialName;

				// マテリアル名で検索し、マテリアルを割り当てる
				auto itr = materials_.find(materialName);
				if (itr != materials_.end()) {
					mesh->SetMaterial(itr->second);
				}
			}
		}
		// 先頭文字列がfならポリゴン
		if (key == "f") {
			int faceIndexCount = 0;
			// 半角スペース区切りで行の続きを読み込む
			string index_string;
			while (getline(line_stream, index_string, ' ')) {
				// 頂点インデックス1個分の文字列をストリームに変換して解析しやすくする
				std::istringstream index_stream(index_string);
				unsigned short indexPosition, indexNormal, indexTexcoord;
				// 頂点番号
				index_stream >> indexPosition;

				Material* material = mesh->GetMaterial();
				index_stream.seekg(1, ios_base::cur);	// スラッシュを飛ばす
				// マテリアル、テクスチャがある場合
				if (material && material->textureFilename_.size() > 0) {
					index_stream >> indexTexcoord;
					index_stream.seekg(1, ios_base::cur);	// スラッシュを飛ばす
					index_stream >> indexNormal;
					// 頂点データの追加
					Mesh::VertexPosNormalUv vertex{};
					vertex.pos = positions[indexPosition - 1];
					vertex.normal = normals[indexNormal - 1];
					vertex.uv = texcoords[indexTexcoord - 1];
					mesh->AddVertex(vertex);
					//// エッジ平滑化用のデータを追加
					//if (smoothing) {
					//	mesh->AddSmoothData(
					//		indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
					//}
				}
				else {
					char c;
					index_stream >> c;
					// スラッシュ2連続の場合、頂点番号のみ
					if (c == '/') {
						// 頂点データの追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = { 0.0f, 0.0f, 1.0f };
						vertex.uv = { 0.0f, 0.0f };
						mesh->AddVertex(vertex);
					} else {
						index_stream.seekg(-1, ios_base::cur); // 1文字戻る
						index_stream >> indexTexcoord;
						index_stream.seekg(1, ios_base::cur);  // スラッシュを飛ばす
						index_stream >> indexNormal;
						// 頂点データを追加
						Mesh::VertexPosNormalUv vertex{};
						vertex.pos = positions[indexPosition - 1];
						vertex.normal = normals[indexNormal - 1];
						vertex.uv = { 0.0f, 0.0f };
						mesh->AddVertex(vertex);
						// エッジ平滑化用のデータを追加
						/*if (smoothing) {
							mesh->AddSmoothData(
								indexPosition, (unsigned short)mesh->GetVertexCount() - 1);
						}*/
					}
				}
				// インデックスデータの追加
				if (faceIndexCount >= 3) {
					// 四角形ポリゴンの4点目なので、
					// 四角形の0, 1, 2, 3の内 2, 3, 0で三角形を構築する
					mesh->AddIndex(unsigned short(indexCountTex - 1));
					mesh->AddIndex((unsigned short)indexCountTex);
					mesh->AddIndex(unsigned short(indexCountTex - 3));
				}
				else {
					mesh->AddIndex((unsigned short)indexCountTex);
				}
				indexCountTex++;
				faceIndexCount++;
			}
		}
	}
	file.close();

	// 頂点法線の平均によるエッジの平滑化
	/*if (smoothing) {
		mesh->CalculateSmoothVertexNormals();
	}*/
}

void Model::LoadMaterial(const std::string& directoryPath, const std::string& filename) {
	// ファイルストリーム
	std::ifstream file;
	// マテリアルファイルを開く
	file.open(directoryPath + filename);
	// ファイルオープン失敗をチェック
	if (file.fail()) {
		assert(0);
	}

	Material* material = nullptr;

	// 1行ずつ読み込む
	string line;
	while (getline(file, line)) {

		// 1行分の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		// 半角スペース区切りで行の先頭文字列を取得
		string key;
		getline(line_stream, key, ' ');

		// 先頭のタブ文字は無視する
		if (key[0] == '\t') {
			key.erase(key.begin()); // 先頭の文字を削除
		}

		// 先頭文字列がnewmtlならマテリアル名
		if (key == "newmtl") {

			// 既にマテリアルがあるなら
			if (material) {
				// マテリアルをコンテナに登録
				AddMaterial(material);
			}

			// 新しいマテリアルを生成
			material = Material::Create();
			// マテリアル名読み込み
			line_stream >> material->name_;
		}
		// 先頭文字列がKaならアンビエント色
		if (key == "Ka") {
			line_stream >> material->ambient_.x >> material->ambient_.y >> material->ambient_.z;
		}
		// 先頭文字列がKdならディフューズ色
		if (key == "Kd") {
			line_stream >> material->diffuse_.x >> material->diffuse_.y >> material->diffuse_.z;
		}
		// 先頭文字列がKsならスペキュラー色
		if (key == "Ks") {
			line_stream >> material->specular_.x >> material->specular_.y >> material->specular_.z;
		}
		// 先頭文字列がmap_Kdならテクスチャファイル名
		if (key == "map_Kd") {
			// テクスチャのファイル名読み込み
			line_stream >> material->textureFilename_;

			// フルパスからファイル名を取り出す
			size_t pos1;
			pos1 = material->textureFilename_.rfind('\\');
			if (pos1 != string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(
					pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}

			pos1 = material->textureFilename_.rfind('/');
			if (pos1 != string::npos) {
				material->textureFilename_ = material->textureFilename_.substr(
					pos1 + 1, material->textureFilename_.size() - pos1 - 1);
			}
		}
	}
	// ファイルを閉じる
	file.close();

	if (material) {
		// マテリアルを登録
		AddMaterial(material);
	}


}

void Model::AddMaterial(Material* material) {
	// コンテナに登録
	materials_.emplace(material->name_, material);
}

void Model::LoadTextures() {
	int textureIndex = 0;
	string directoryPath = name_ + "/";

	for (auto& m : materials_) {
		Material* material = m.second;
		
		// テクスチャあり
		if (material->textureFilename_.size() > 0) {
			// マテリアルにテクスチャ読み込み
			material->LoadTexture(directoryPath);
			textureIndex++;
		}
		// テクスチャなし
		else {
			// マテリアルにテクスチャ読み込み
			material->LoadTexture("white1x1.png");
			textureIndex++;
		}
	}
}


