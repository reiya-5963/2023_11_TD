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

	Retry();

	backTex_ = TextureManager::Load("skydomeTex.png");
	back_.reset(Sprite::Create(backTex_, { 640.0f, 320.0f }, 0.0f, {0.8f, 0.8f, 0.8f, 1.0f}, {0.5f, 0.5f}));

	retryPos_ = { 1280.0f -  0.4f, 0.4f };
	retryTex_ = TextureManager::Load("Rretry.png");
	retry_.reset(Sprite::Create(retryTex_, retryPos_, 0.0f, { 0.8f, 0.8f, 0.8f, 1.0f }, { 1.0f, 0.0f }));
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

	//back_->Update();

	Mapchip::GetInstance()->Update(viewProjection_);

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		if (joyState.Gamepad.wButtons == XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			Retry();
		}
	}
	else if (Input::GetInstance()->TriggerKey(DIK_R)) {
		Retry();
	}
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	////この間に背景スプライトの描画を入れる


	back_->Draw();


	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(commandList, Model::BlendMode::kNone);
	
	map_->Draw(viewProjection_);
	playerController_->Draw(viewProjection_);

	gimmickManager_->Draw(viewProjection_);

	colliderManager_->Draw(viewProjection_);
	//back_->Draw(viewProjection_);


	Model::PostDraw();

	Sprite::PreDraw(commandList);
	//

	retry_->Draw();
	



	Sprite::PostDraw();

	//ParticleManager::Draw(commandList, ParticleManager::BlendMode::kAdd);
}

void GameScene::Retry() {
	
	focusCamera_.reset(new FocusCamera());
	focusCamera_->Initialize();
	focusCamera_->SetPosition(Vector3{ 0,15.0f,-34.0f });

	focusCamera_->SettingAnimation(focusCamera_->GetView().translation_, Vector3(10.0f, 15.0f, -50.0f));

	colliderManager_.reset(new CollisionManager());
	colliderManager_->Initialize();

	map_ = Mapchip::GetInstance();
	map_->Initialize("resources/Level/Level_1_.csv");

	playerController_.reset(new PlayerController());
	playerController_->Initialize();

	gimmickManager_.reset(new GimmickManager());
	gimmickManager_->Initialize();
	playerController_->SetGimmickManager(gimmickManager_.get());

	if (typeid(*playerController_->GetPlayerPtr1()->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetParent(playerController_->GetPlayerPtr1()->GetWorldTransform());
	}
	else {
		focusCamera_->SetParent(playerController_->GetPlayerPtr2()->GetWorldTransform());
	}

	//skyModel.reset(Model::CreateFlomObj("skydome"));
	//back_ = std::make_unique<Skydome>();
	//back_->Initialize(skyModel.get(), {0.0f, 0.0f ,0.0f});



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
	if (typeid(*playerController_->GetPlayerPtr1()->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetParent(playerController_->GetPlayerPtr1()->GetWorldTransform());
	}
	else {
		focusCamera_->SetParent(playerController_->GetPlayerPtr2()->GetWorldTransform());
	}
	focusCamera_->Update();
	viewProjection_.matProjection = focusCamera_->GetView().matProjection;
	viewProjection_.matView = focusCamera_->GetView().matView;
	viewProjection_.TransferMatrix();

}