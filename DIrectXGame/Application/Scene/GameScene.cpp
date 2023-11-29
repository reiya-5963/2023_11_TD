#include "GameScene.h"
#include <cassert>
#include "R_Math.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include "SceneManager.h"
#include "Player/Player.h"
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
	audio_ = Audio::GetInstance();
	// テクスチャの読み込み
	//textureHandle_ = TextureManager::Load("sample.png");
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	goalModel_.reset(Model::CreateFlomObj("cube"));
	goal_ = std::make_unique<Goal>();
	goal_->Initialize(goalModel_.get());

	focusCamera_.reset(new FocusCamera());
	colliderManager_.reset(new CollisionManager());
	playerController_.reset(new PlayerController());
	gimmickManager_.reset(new GimmickManager());
	map_ = Mapchip::GetInstance();
	map_->Initialize("resources/Level/Level_1_.csv");

	playerController_->Initialize();
	focusCamera_->Initialize();
	colliderManager_->Initialize();
	gimmickManager_->Initialize();
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


	gimmickManager_->Update();

	playerController_->Update(viewProjection_);

	this->ColliderUpdate();


	//back_->Update();

	goal_->Update();

	Mapchip::GetInstance()->Update(viewProjection_);

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		if ((joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER)) {
			Retry();
		}
	}
	else if (Input::GetInstance()->TriggerKey(DIK_R)) {
		Retry();
	}

	if (playerController_->GetIsClear()) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
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

	goal_->Draw(viewProjection_);

	Model::PostDraw();

	Sprite::PreDraw(commandList);
	//


	playerController_->UIDraw();
	retry_->Draw();
	



	Sprite::PostDraw();

	//ParticleManager::Draw(commandList, ParticleManager::BlendMode::kAdd);
}

void GameScene::Retry() {
	
	focusCamera_->SetPosition(Vector3{ 0,15.0f,-34.0f });

	focusCamera_->SettingAnimation(focusCamera_->GetView().translation_, Vector3(10.0f, 15.0f, -50.0f));

	playerController_->Reset();

	playerController_->SetGimmickManager(gimmickManager_.get());

	//if (typeid(*playerController_->GetPlayerPtr1()->GetInputState()) == typeid(ActiveState)) {
	//	focusCamera_->SetParent(playerController_->GetPlayerPtr1()->GetWorldTransform());
	//}
	//else {
	//	focusCamera_->SetParent(playerController_->GetPlayerPtr2()->GetWorldTransform());
	//}

	const float xCenter = 48 / 2.0f - 0.5f;
	Vector3 GoalPosition = { ((47.0f - 1.0f) - xCenter) * 2.0f, (23.0f * 2.0f + 1.0f), 0.0f };

	goal_->SetPosition(GoalPosition);
	playerController_->SetGoalPosition(GoalPosition);

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

	colliderManager_->AddColliders(goal_.get());
	// 当たり判定チェック
	colliderManager_->CheckAllCollisions();
	colliderManager_->UpdateWorldTransform();

}

void GameScene::CameraUpdate()
{
	//if (typeid(*playerController_->GetPlayerPtr1()->GetInputState()) == typeid(ActiveState)) {
	//	focusCamera_->SetParent(playerController_->GetPlayerPtr1()->GetWorldTransform());
	//}
	//else {
	//	focusCamera_->SetParent(playerController_->GetPlayerPtr2()->GetWorldTransform());
	//}
	
	Vector3 world1Pos{};
	Vector3 world2Pos{};

	/*if(playerController_->)*/
	
	if (typeid(*playerController_->GetPlayer1()->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetIsControlPlayer1(true);
	}
	else if (typeid(*playerController_->GetPlayer2()->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetIsControlPlayer1(false);
	}

	world1Pos = playerController_->GetPlayer1()->GetWorldPosition();
	world2Pos = playerController_->GetPlayer2()->GetWorldPosition();

	focusCamera_->SetPlayerWorld2ScreenPos(world1Pos, world2Pos);
	focusCamera_->Update();
	viewProjection_.matProjection = focusCamera_->GetView().matProjection;
	viewProjection_.matView = focusCamera_->GetView().matView;
	viewProjection_.TransferMatrix();

}