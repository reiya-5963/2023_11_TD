#include "PlayerController.h"
#include <limits>
#include <imgui.h>
#include "CollisionTypeIdDef.h"
#include "WinApp.h"
#include "FocusCamera.h"
#include "AudioManager/AudioManager.h"

// マクロの停止
#undef max

PlayerController::PlayerController()
{
	jumpSE_ = Audio::GetInstance()->LoadWave("sound/JumpSE.wav");
	ghostSE_ = Audio::GetInstance()->LoadWave("sound/GhostSE.wav");
	switchSE_ = Audio::GetInstance()->LoadWave("sound/SwitchSE.wav");
}

PlayerController::~PlayerController()
{
}

void PlayerController::Initialize()
{
	audio_ = Audio::GetInstance();

	player1_ = std::make_unique<Player>();
	player2_ = std::make_unique<Player>();

	Model* p_model1;
	p_model1 = Model::CreateFlomObj("ghostWhite");
	Model* p_model2;
	p_model2 = Model::CreateFlomObj("ghostBlack");

	std::vector<Model*> p_models1 = {
		p_model1
	};
	std::vector<Model*> p_models2 = {
		p_model2
	};

	Vector3 position = { 0,3,0 };
	player1_->Initialize(p_models1);
	player1_->Setting(position, 0);
	player1_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer1));

	position = { 2.5f,3,0 };
	player2_->Initialize(p_models2);
	player2_->Setting(position, 0);
	player2_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer2));

	player1_->SetState(new ActiveState());
	player2_->SetState(new InactiveState());

	float uiScale = 6.0f;

	uint32_t uiTexture = TextureManager::Load("UI/BButton.png");
	BButtonUi_.reset(Sprite::Create(uiTexture, { 100,100 }, 0.0f, { 1,1,1,1.0f }, { 0.5f,0.5f }));
	BButtonUi_->SetSize({ BButtonUi_->GetSize().x / uiScale,BButtonUi_->GetSize().y / uiScale });

	uiTexture = TextureManager::Load("UI/XButton.png");
	XButtonUi_.reset(Sprite::Create(uiTexture, { 0,0 }, 0.0f, { 1,1,1,1 }, { 0.5f,0.5f }));
	XButtonUi_->SetSize({ XButtonUi_->GetSize().x / uiScale,XButtonUi_->GetSize().y / uiScale });

	uiTexture = TextureManager::Load("UI/Arrow.png");
	ArrowUi_.reset(Sprite::Create(uiTexture, { 0,0 }, 0.0f, { 1,1,1,1 }, { 0.5f,0.5f }));
	ArrowUi_->SetSize({ ArrowUi_->GetSize().x / 3,ArrowUi_->GetSize().y / 3 });

}

void PlayerController::Update(const ViewProjection& viewProjection)
{

	Vector3 world1Pos = player1_->GetWorldPosition();
	Vector3 world2Pos = player2_->GetWorldPosition();
	Vector2 player1ScreenPos_ = World2ScreenPos(viewProjection, world1Pos);
	Vector2 player2ScreenPos_ = World2ScreenPos(viewProjection, world2Pos);

	if (typeid(*player1_->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetIsControlPlayer1(true);
	}
	else if (typeid(*player2_->GetInputState()) == typeid(ActiveState)) {
		focusCamera_->SetIsControlPlayer1(false);
	}

#pragma region カメラ
	if (!focusCamera_->IsEase() && focusCamera_->IsControlPlayer1()) {


		if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kLeftBottom]) {
			if (isLeavePlayer_ && player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kRightBottom]) {
			if (isLeavePlayer_ && player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f &&
				player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kLeftTop]) {
			if (isLeavePlayer_ && player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kRightTop]) {
			if (isLeavePlayer_ && player1ScreenPos_.y < float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f &&
				player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kMiddleBottom]) {
			if (player1ScreenPos_.x < 0.0f &&
				player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kMiddleTop]) {
			if (player1ScreenPos_.x < 0.0f &&
				player1ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player1ScreenPos_.y > 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
		}
	}
	else if (!focusCamera_->IsEase() && !focusCamera_->IsControlPlayer1()) {

		if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kLeftBottom]) {
			if (isLeavePlayer_ && player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kRightBottom]) {
			if (isLeavePlayer_ && player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f &&
				player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kLeftTop]) {
			if (isLeavePlayer_ && player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kRightTop]) {
			if (isLeavePlayer_ && player2ScreenPos_.y < float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (isLeavePlayer_) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f &&
				player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kMiddleBottom]) {
			if (player2ScreenPos_.x < 0.0f &&
				player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleTop);
				focusCamera_->SetIsEase(true);
			}
		}
		else if (focusCamera_->GetView().translation_ == focusCamera_->GetCenters()[FocusCamera::CameraPoint::kMiddleTop]) {
			if (player2ScreenPos_.x < 0.0f &&
				player2ScreenPos_.y < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
				player2ScreenPos_.y > 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightBottom);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x < 0.0f) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kLeftTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kRightTop);
				focusCamera_->SetIsEase(true);
			}
			else if (player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
				focusCamera_->SetCameraPoint(FocusCamera::CameraPoint::kMiddleBottom);
				focusCamera_->SetIsEase(true);
			}
		}
	}

	// もし前フレームで操作プレイヤーが違うなら
	if (focusCamera_->IsPreControlPlayer1() != focusCamera_->IsControlPlayer1() && !focusCamera_->IsEase()) {
		focusCamera_->SetIsEase(true);
		if (std::abs(world2Pos.x - world1Pos.x) > 48.0f) {
			isLeavePlayer_ = true;
		}
		else {
			isLeavePlayer_ = false;
		}
	}
#pragma endregion

	if (player1_->GetIsGoal() && player2_->GetIsGoal()) {
		isGoalAnimation_ = true;
		player1_->SetIsGoalAnimation(isGoalAnimation_);
		player2_->SetIsGoalAnimation(isGoalAnimation_);
		//isClear_ = true;
	}

	// 遅延（バグ防止の仮
	if (ghostWork_.isDelay_) {
		int interval = 60;
		ghostWork_.delayCount_++;
		if (ghostWork_.delayCount_ > interval) {
			ghostWork_ = {};
		}
	}

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		bool inputKey = (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && (inactivePlayer_->GetBehaviorState() != Player::Behavior::kAction));
		if (inputKey && !ghostWork_.isDelay_ && activePlayer_->GetBehaviorState() != Player::Behavior::kJump) {
#pragma region ギミックに憑りつく
			ghostWork_.isDelay_ = true;

			float length = std::numeric_limits<float>::max();
			IGimmick* tmpGimmick = nullptr;
			Player* tmpPlayer = nullptr;
			if (typeid(*player1_->GetInputState()) == typeid(ActiveState)) {
				tmpPlayer = player1_.get();
			}
			else {
				tmpPlayer = player2_.get();
			}

			// 範囲設定
			float playerRadius = 5.0f;
			float objectRadius = 1.0f;
			Vector3 playerPosition = tmpPlayer->GetWorldPosition();

			Vector2_AABB player = {
				{playerPosition.x - playerRadius,playerPosition.y - playerRadius},
				{playerPosition.x + playerRadius,playerPosition.y + playerRadius},
			};
			Vector2_AABB gim = {};
			for (IGimmick* gimmick : gimmickManager_->GetGimmickList()) {
				// 親を所持している場合スキップ
				if (gimmick->GetWorldTransform()->parent_ != nullptr) {
					continue;
				}
				// 長さ用
				Vector3 range = R_Math::Subtract(playerPosition, gimmick->GetWorldPosition());
				// オブジェクトのAABB
				gim = {
					{gimmick->GetWorldPosition().x - objectRadius, gimmick->GetWorldPosition().y - objectRadius},
					{gimmick->GetWorldPosition().x + objectRadius, gimmick->GetWorldPosition().y + objectRadius},
				};
				if (!IsAABBCollision(player, gim)) {
					// No
					continue;
				}

				// 最短距離を計算
				if (length > R_Math::Length(range)) {
					length = R_Math::Length(range);
					tmpGimmick = gimmick;
				}

			}

			// 見つかれば呼び出し
			if (tmpGimmick != nullptr && !tmpGimmick->GetIsSetup()) {
				AudioManager::GetInstance()->PlaySEAudio(AudioManager::GetSoundList(AudioManager::kSwicth), 0.1f);
				tmpPlayer->SetGimmickPtr(tmpGimmick);
				tmpPlayer->SetBehaviorRequest(Player::Behavior::kAction);
				tmpPlayer->GhostSetting();
				if (this->controlNum_ == ControlNum::kOne) {
					this->changeRequest_ = ControlNum::kTwo;
				}
				else {
					this->changeRequest_ = ControlNum::kOne;
				}
			}
			else {
				ghostWork_.isDelay_ = false;
			}
#pragma endregion
		}
	}

	ChangeControl();
	
	player1_->Update();
	player2_->Update();
	// ボタン表示
	ActivePlayerArea(viewProjection);
	InactivePlayerInfo(viewProjection);

	if (!player1_->IsGoalAnimation() && !player2_->IsGoalAnimation() &&
		player1_->GetIsGoal() && player2_->GetIsGoal()) {
		isClear_ = true;
	}

	// ゴールへの矢印
	Vector3 arrowWorld = R_Math::Subtract(goalPosition_, activePlayer_->GetWorldPosition());
	arrowWorld = R_Math::Normalize(arrowWorld);
	ArrowUi_->SetRotate(-std::atan2f(arrowWorld.y, arrowWorld.x));
	float scaler = 5.0f;
	arrowWorld = { arrowWorld.x * scaler,arrowWorld.y * scaler,arrowWorld.z * scaler };
	Vector2 arrowScreen = this->GenerateScreenPosition(activePlayer_->GetWorldPosition(), arrowWorld, viewProjection);
	ArrowUi_->SetPosition(arrowScreen);
}

void PlayerController::Draw(ViewProjection& viewprojection)
{
	player1_->Draw(viewprojection);
	player2_->Draw(viewprojection);
}

void PlayerController::ChangeControl()
{
	XINPUT_STATE joyState;

	if (isDelay_) {
		int kChangeFrame = 30;
		if (delayCount_++ >= kChangeFrame) {
			delayCount_ = 0;
			isDelay_ = false;
		}
	}

	float seVolume = 0.1f;

	// 切り替えキー
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X && !isDelay_) {
			switch (controlNum_)
			{
			case PlayerController::ControlNum::kOne:
				if (player2_->GetBehaviorState() == Player::Behavior::kAction) {
					break;
				}
				this->changeRequest_ = ControlNum::kTwo;
				AudioManager::GetInstance()->PlaySEAudio(AudioManager::GetSoundList(AudioManager::kSwicth), seVolume);
				break;
			case PlayerController::ControlNum::kTwo:
				if (player1_->GetBehaviorState() == Player::Behavior::kAction) {
					break;
				}
				this->changeRequest_ = ControlNum::kOne;
				AudioManager::GetInstance()->PlaySEAudio(AudioManager::GetSoundList(AudioManager::kSwicth), seVolume);
				break;
			}
		}
	}

	if (changeRequest_) {
		// 変更
		controlNum_ = changeRequest_.value();
		isDelay_ = true;

		switch (controlNum_)
		{
		case PlayerController::ControlNum::kOne:
			player1_->SetState(new ActiveState());
			player2_->SetState(new InactiveState());

			break;
		case PlayerController::ControlNum::kTwo:
			player1_->SetState(new InactiveState());
			player2_->SetState(new ActiveState());
			break;
		}
		// リクエストリセット
		changeRequest_ = std::nullopt;
	}


}

void PlayerController::ActivePlayerArea(const ViewProjection& viewProjection)
{
	isInArea_ = false;
	// アクティブ中
	PtrSetting();

	isInArea_ = this->PlayerInGimmick(activePlayer_);
	activePlayer_->SetIsInArea(isInArea_);

	if (isInArea_) {
		Vector3 offset = { 0.0f,2.0f,0.0f };
		BButtonUi_->SetPosition(GenerateScreenPosition(activePlayer_->GetWorldPosition(), offset, viewProjection));
	}

}

void PlayerController::InactivePlayerInfo(const ViewProjection& viewProjection)
{
	if (inactivePlayer_ == nullptr) {
		return;
	}

	Vector3 offset = { 1.0f,1.5f,0.0f };
	this->XButtonUi_->SetPosition(GenerateScreenPosition(inactivePlayer_->GetWorldPosition(), offset, viewProjection));

}

bool PlayerController::PlayerInGimmick(Player* player)
{
	Vector3 worldPosition = player->GetWorldPosition();

	// AABB
	float playerRadius = 5.0f;
	Vector2_AABB gim = {};
	Vector2_AABB aabb = {
		{worldPosition.x - playerRadius,worldPosition.y - playerRadius},
		{worldPosition.x + playerRadius,worldPosition.y + playerRadius},
	};

	for (IGimmick* gimmick : gimmickManager_->GetGimmickList()) {
		// 親を所持している場合スキップ
		if (gimmick->GetWorldTransform()->parent_ != nullptr) {
			continue;
		}
		// オブジェクトのAABB
		gim_ = {
			{gimmick->GetWorldPosition().x - 1.0f, gimmick->GetWorldPosition().y - 1.0f},
			{gimmick->GetWorldPosition().x + 1.0f, gimmick->GetWorldPosition().y + 1.0f},
		};
		if (!IsAABBCollision(aabb, gim_)) {
			// No
			continue;
		}
		if (gimmick->GetIsSetup()) {
			continue;
		}

		return true;
	}

	return false;
}

Vector2 PlayerController::World2ScreenPos(const ViewProjection& viewProjection, Vector3& worldPos1) {
	Matrix4x4 matViewPort =
		R_Math::MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	Matrix4x4 matViewProjectionViewPort = R_Math::Multiply(
		viewProjection.matView, R_Math::Multiply(viewProjection.matProjection, matViewPort));

	Vector3 tmpPlayer1Pos = R_Math::TransformCoord(worldPos1, matViewProjectionViewPort);

	Vector2 player1ScreenPos_ = { tmpPlayer1Pos.x, tmpPlayer1Pos.y };
	return player1ScreenPos_;
}

void PlayerController::UIDraw()
{
	if (isInArea_) {
		this->BButtonUi_->Draw();
	}

	if (inactivePlayer_->GetBehaviorState() != Player::Behavior::kAction) {
		this->XButtonUi_->Draw();
	}

	ArrowUi_->Draw();

}

void PlayerController::Reset()
{
	Vector3 position = { 0,3,0 };
	player1_->Setting(position, 0);
	player1_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer1));

	position = { 2.5f,3,0 };
	player2_->Setting(position, 0);
	player2_->SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kPlayer2));

	player1_->SetState(new ActiveState());
	player2_->SetState(new InactiveState());

	isInArea_ = false;
	isClear_ = false;

	controlNum_ = ControlNum::kOne;
}

void PlayerController::PtrSetting()
{
	activePlayer_ = nullptr;
	inactivePlayer_ = nullptr;

	// アクティブ中
	if (typeid(*player1_->GetInputState()) == typeid(ActiveState)) {
		activePlayer_ = player1_.get();
		inactivePlayer_ = player2_.get();
	}
	else {
		activePlayer_ = player2_.get();
		inactivePlayer_ = player1_.get();
	}
}

Player* PlayerController::GetPlayer1()
{
	return player1_.get();
}

Player* PlayerController::GetPlayer2()
{
	return player2_.get();
}

Vector3 PlayerController::WorldToScreen(const Vector3& position, const ViewProjection& viewProjection)
{
	// ビューポート行列
	Matrix4x4 matViewport = R_Math::MakeViewPortMatrix(0, 0,
		(float)WinApp::kWindowWidth, (float)WinApp::kWindowHeight, 0, 1);
	// ビュープロジェクション
	Matrix4x4 matViewProjection = R_Math::Multiply(
		R_Math::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	return Vector3(R_Math::TransformCoord(position, matViewProjection));
}

Vector2 PlayerController::GenerateScreenPosition(const Vector3& worldPosition, const Vector3& offset, const ViewProjection& viewProjection)
{

	Vector3 positionWorld = R_Math::Add(worldPosition, offset);

	Vector3 positionScreen = WorldToScreen(positionWorld, viewProjection);

	Vector2 screen = { positionScreen.x,positionScreen.y };

	return screen;
}

bool PlayerController::IsAABBCollision(const Vector2_AABB& aabb1, const Vector2_AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&	// X軸
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y))	// Y軸
	{	// Z軸
		return true;
	}
	return false;
}
