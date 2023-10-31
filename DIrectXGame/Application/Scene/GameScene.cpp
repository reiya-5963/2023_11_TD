#include "GameScene.h"
#include <cassert>
#include "R_Math.h"
#include "TextureManager.h"
#include "ImGuiManager.h"

/// <summary>
/// コンストラクタ
/// </summary>
GameScene::GameScene() {}

/// <summary>
/// デストラクタ
/// </summary>
GameScene::~GameScene() {
}

void GameScene::Initialize() {
	dxCommon_ = DirectXCommon::GetInstance();

	// テクスチャの読み込み
	//textureHandle_ = TextureManager::Load("sample.png");
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	colliderManager_ = std::make_unique<CollisionManager>();
	colliderManager_->Initialize();
}

void GameScene::Finalize() {}

void GameScene::Update() {}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	////この間に背景スプライトの描画を入れる
	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(commandList, Model::BlendMode::kNone);


	Model::PostDraw();

	Sprite::PreDraw(commandList);
	//






	Sprite::PostDraw();

	//ParticleManager::Draw(commandList, ParticleManager::BlendMode::kAdd);

	ParticleManager::PreDraw(commandList);
	ParticleManager::PostDraw();
}

