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

	focusCamera_ = std::make_unique<FocusCamera>();
	focusCamera_->Initialize();
	focusCamera_->SetPosition(Vector3{ 0,15.0f,-40.0f });

	focusCamera_->SettingAnimation(focusCamera_->GetView().translation_, Vector3(10.0f, 15.0f, -50.0f));

	colliderManager_ = std::make_unique<CollisionManager>();
	colliderManager_->Initialize();

	map_ = Mapchip::GetInstance();
	map_->Initialize("resources/Level/Level1.csv");


	playerController_ = std::make_unique<PlayerController>();
	playerController_->Initialize();

	gimmickManager_ = std::make_unique<GimmickManager>();
	gimmickManager_->Initialize();
	playerController_->SetGimmickManager(gimmickManager_.get());

}

void GameScene::Finalize() {
	
}

void GameScene::Update() {
	
	if (Input::GetInstance()->TriggerKey(DIK_9)) {
		focusCamera_->SetIsAnimater(true);
	}

	CameraUpdate();
	playerController_->Update();

	gimmickManager_->Update();

	this->ColliderUpdate();



}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	////この間に背景スプライトの描画を入れる
	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(commandList, Model::BlendMode::kNone);
	
	map_->Draw(viewProjection_);
	playerController_->Draw(viewProjection_);

	gimmickManager_->Draw(viewProjection_);

	colliderManager_->Draw(viewProjection_);


	Model::PostDraw();

	Sprite::PreDraw(commandList);
	//


	



	Sprite::PostDraw();

	//ParticleManager::Draw(commandList, ParticleManager::BlendMode::kAdd);
}

void GameScene::ColliderUpdate() {
	// コライダーリストの初期化
	colliderManager_->ClearColliders();

	colliderManager_->AddColliders(playerController_->GetPlayer1());
	colliderManager_->AddColliders(playerController_->GetPlayer2());

	std::list<IGimmick*> gimmicks = gimmickManager_->GetGimmicks();

	for (IGimmick* gimmick : gimmicks) {
		colliderManager_->AddColliders(gimmick);
	}
	// 当たり判定チェック
	colliderManager_->CheckAllCollisions();
	colliderManager_->UpdateWorldTransform();

}

void GameScene::CameraUpdate()
{

	focusCamera_->Update();
	viewProjection_.matProjection = focusCamera_->GetView().matProjection;
	viewProjection_.matView = focusCamera_->GetView().matView;
	viewProjection_.TransferMatrix();

}