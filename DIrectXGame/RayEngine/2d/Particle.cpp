//#include "Particle.h"
//#include "R_Math.h"
//#include <cassert>
//#include "TextureManager.h"
//#include "MyD3D12Create.h"
////#pragma comment(lib, "dxcompiler.lib")
//#include "DXCManager.h"
//#include "ParticleManager.h"
//
///// <summary>
///// コンストラクタ
///// </summary>
//Particle::Particle() {}
//
///// <summary>
///// コンストラクタ
///// </summary>
///// <param name="position">座標</param>
///// <param name="rotate">回転</param>
///// <param name="scale">大きさ</param>
//Particle::Particle(uint32_t textureHandle, Vector2 position, float rotate, Vector2 size,
//	Vector4 color, Vector2 anchorPoint, bool isFlipX, bool isFlipY) {
//	position_ = position;
//	rotation_ = rotate;
//	size_ = size;
//	anchorPoint_ = anchorPoint;
//	matWorld_ = R_Math::MakeIdentity4x4();
//	color_ = color;
//	textureHandle_ = textureHandle;
//	isFlipX_ = isFlipX;
//	isFlipY_ = isFlipY;
//	texSize_ = size;
//}
//
///// <summary>
///// 初期化
///// </summary>
///// <returns>できたかどうか</returns>
//bool Particle::Initialize() {
//	HRESULT result = S_FALSE;
//
//	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
//
//	{//vertexResourceの生成
//		D3D12_HEAP_PROPERTIES heapProps = MyCreate::SetHeapPro(D3D12_HEAP_TYPE_UPLOAD);
//		D3D12_RESOURCE_DESC resourceDesc = MyCreate::SetBufferResourceDesc(sizeof(VertexPosUv) * 4);
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
//	vertexBufferView_.SizeInBytes = sizeof(VertexPosUv) * 4;
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
//	return true;
//}
//
///// <summary>
///// 描画
///// </summary>
///// <param name="viewProMat">ViewProjectionMatrix</param>
//void Particle::Draw(ID3D12GraphicsCommandList* commandList) {
//	//ワールド行列の計算
////Matrix4x4 TransWorld = R_Math::MakeAffineMatrix(worldTrans_.scale, worldTrans_.rotate, worldTrans_.translate);
//	matWorld_ = R_Math::MakeIdentity4x4();
//	matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeRotateZMatrix(rotation_));
//	matWorld_ = R_Math::Multiply(matWorld_, R_Math::MakeTranslateMatrix({ position_.x, position_.y, 0.0f }));
//
//
//	//ワールド行列とビュープロジェクション行列の合成
//	//matWorld_ = R_Math::Multiply(matWorld_, viewProMat);
//
//	//それぞれ定数バッファに代入
//	constMap_->color = color_;
//	constMap_->mat = R_Math::Multiply(matWorld_, ParticleManager::GetMatProjection());
//
//	//頂点バッファの設定
//	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
//	//定数バッファビューをセット
//	commandList->SetGraphicsRootConstantBufferView(0, constBuffer_->GetGPUVirtualAddress());
//}
//
///// <summary>
///// 頂点データ転送
///// </summary>
//void Particle::TransferVertices() {
//	//HRESULT result = S_FALSE;
//
//		//左下、左上、右下、右上
//	enum { LB, LT, RB, RT };
//
//	float left = (0.0f - anchorPoint_.x) * size_.x;
//	float right = (1.0f - anchorPoint_.x) * size_.x;
//	float top = (0.0f - anchorPoint_.y) * size_.y;
//	float bottom = (1.0f - anchorPoint_.y) * size_.y;
//
//	if (isFlipX_) {
//		left = -left;
//		right = -right;
//	}
//
//	if (isFlipY_) {
//		top = -top;
//		bottom = -bottom;
//	}
//
//
//	VertexPosUv vertices[4];
//
//	vertices[LB].pos = { left, bottom, 0.0f };
//	vertices[LT].pos = { left, top, 0.0f };
//	vertices[RB].pos = { right, bottom, 0.0f };
//	vertices[RT].pos = { right, top, 0.0f };
//
//	{
//		float texLeft = texBase_.x / resourceDesc_.Width;
//		float texRight = (texBase_.x + texSize_.x) / resourceDesc_.Width;
//		float texTop = texBase_.y / resourceDesc_.Height;
//		float texBottom = (texBase_.y + texSize_.y) / resourceDesc_.Height;
//
//		vertices[LB].uv = { texLeft, texBottom };
//		vertices[LT].uv = { texLeft, texTop };
//		vertices[RB].uv = { texRight, texBottom };
//		vertices[RT].uv = { texRight, texTop };
//	}
//
//	//バッファに転送
//	memcpy(vertexMap_, vertices, sizeof(vertices));
//}
//
//void Particle::SetSize(const Vector2& size) {
//	size_ = size;
//	TransferVertices();
//}
//
//void Particle::SetPosition(const Vector2& position) {
//	position_ = position;
//	TransferVertices();
//}
//
//void Particle::SetRotate(const float& rotate) {
//	rotation_ = rotate;
//	TransferVertices();
//}
//
//void Particle::SetColor(const Vector4& color) {
//	color_ = color;
//}
//
//void Particle::SetTextureRect(const Vector2& texBase, const Vector2 texSize) {
//	texBase_ = texBase;
//	texSize_ = texSize;
//	TransferVertices();
//}
