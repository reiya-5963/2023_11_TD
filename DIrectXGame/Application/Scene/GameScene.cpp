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
	Audio::GetInstance()->StopWave(audioManager_->GetSoundList(AudioManager::kAllBGM));
}

void GameScene::Initialize() {
	// インスタンス
	dxCommon_ = DirectXCommon::GetInstance();
	audio_ = Audio::GetInstance();
	audioManager_ = AudioManager::GetInstance();
	transitionmanager_ = TransitionManager::GetInstance();

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
	playerController_->SetFocusCamera(focusCamera_.get());
	focusCamera_->Initialize();
	colliderManager_->Initialize();
	gimmickManager_->Initialize();
	Retry();
	
	backTex_ = TextureManager::Load("GameSceneSprite/skydomeTex.png");
	back_.reset(Sprite::Create(backTex_, { 640.0f, 320.0f }, 0.0f, {0.8f, 0.8f, 0.8f, 1.0f}, {0.5f, 0.5f}));

	retryPos_ = { 1280.0f -  0.4f, 0.4f };
	retryTex_ = TextureManager::Load("GameSceneSprite/resetb.png");
	retry_.reset(Sprite::Create(retryTex_, retryPos_, 0.0f, { 0.8f, 0.8f, 0.8f, 1.0f }, { 1.0f, 0.0f }));

	Vector2 title = { 1280.0f - 0.4f,0.4f + (retry_->GetSize().y / 1.5f) };
	retryTex_ = TextureManager::Load("GameSceneSprite/TitleBackUI.png");
	titleUI_.reset(Sprite::Create(retryTex_, title, 0, { 0.8f,0.8f,0.8f,1.0f }, { 1.0f,0.0f }));

	isBackTitle_ = false;
}

void GameScene::Finalize() {
	
}

void GameScene::Update() 
{
#pragma region リトライ処理
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		bool keyFlag = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) && (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
		if (keyFlag) {
			Retry();
		}
		
		keyFlag = (joyState.Gamepad.bLeftTrigger && joyState.Gamepad.bRightTrigger);
		if (keyFlag && !isBackTitle_) {
			Audio::GetInstance()->StopWave(audioManager_->GetSoundList(AudioManager::kAllBGM));
			isBackTitle_ = true;
			transitionmanager_->TransitionSetting();
		}

	}
	if (Input::GetInstance()->TriggerKey(DIK_R)) {
		//Retry();
		playerController_->SetIsClear(true);
		ClearBGMSetting();
	}
#pragma endregion


	if (playerController_->GetIsClear()) {
		//SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	else {
		gimmickManager_->Update();

		playerController_->Update(viewProjection_);
		CameraUpdate();

		this->ColliderUpdate();

		goal_->Update();

		Mapchip::GetInstance()->Update(viewProjection_);

	}

	// 遷移後シーン変更
	if (transitionmanager_->GetIsBlackOutHalf()) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}

	// 遷移更新
	transitionmanager_->Update();
}

void GameScene::Draw() {
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	Sprite::PreDraw(commandList);
	////この間に背景スプライトの描画を入れる

	// UI表示
	back_->Draw();


	Sprite::PostDraw();

	dxCommon_->ClearDepthBuffer();

	Model::PreDraw(commandList, Model::BlendMode::kNone);
	
	map_->Draw(viewProjection_);
	playerController_->Draw(viewProjection_);

	gimmickManager_->Draw(viewProjection_);

	colliderManager_->Draw(viewProjection_);

	goal_->Draw(viewProjection_);

	Model::PostDraw();

	Sprite::PreDraw(commandList);


	playerController_->UIDraw();
	retry_->Draw();
	titleUI_->Draw();


	Sprite::PostDraw();

	// 遷移描画
	transitionmanager_->Draw();

}

void GameScene::Retry() {
	
	focusCamera_->SetPosition(Vector3{ 0,15.0f,-34.0f });

	focusCamera_->SettingAnimation(focusCamera_->GetView().translation_, Vector3(10.0f, 15.0f, -50.0f));

	playerController_->Reset();

	playerController_->SetGimmickManager(gimmickManager_.get());

	gimmickManager_->Initialize();

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
	focusCamera_->Update();
	viewProjection_.matProjection = focusCamera_->GetView().matProjection;
	viewProjection_.matView = focusCamera_->GetView().matView;
	viewProjection_.TransferMatrix();

}

void GameScene::ClearBGMSetting()
{
	AudioManager::GetInstance()->PlayBGMAudio(AudioManager::GetSoundList(AudioManager::kGoalSE), false, 0.1f);
	Audio::GetInstance()->StopWave(audioManager_->GetSoundList(AudioManager::kAllBGM));
}
