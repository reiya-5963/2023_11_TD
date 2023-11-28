#include "InputState.h"
#include "Input.h"
#include "R_Math.h"
#include "ImGuiManager.h"

void ActiveState::Update()
{
	if (player_->GetBehaviorState() != Player::Behavior::kAction) {
		ActionInput();
	}
#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_0) &&
		player_->GetBehaviorState() != Player::Behavior::kJump) {
		player_->SetBehaviorRequest(Player::Behavior::kJump);
	}
#endif // _DEBUG
}

void ActiveState::MoveInputProcess()
{
}

void ActiveState::ActionInput()
{
	Vector3 move = {};

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		bool KeyInput = false;
		// 閾値
		const float threshold = 0.7f;
		bool isMoving = false;

		// 移動量計算
		Vector3 moved = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX,0,(float)joyState.Gamepad.sThumbLY / SHRT_MAX
		};
		if (moved.Length() > threshold) {
			isMoving = true;
		}
		// 移動処理
		if (isMoving) {
			const float speed = 10.0f;

			move = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed,0,0
			};

			if (move.x > 0) {
				player_->SetIsLeft(false);
			}
			else {
				player_->SetIsLeft(true);
			}

		}

		//---ジャンプ入力---//
		// ジャンプ中でなければ
		KeyInput = joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		bool playerState = player_->GetBehaviorState() != Player::Behavior::kJump;
		if (KeyInput && playerState && player_->GetIsDriveObject()) {
			player_->SetBehaviorRequest(Player::Behavior::kJump);
		}

	}
	// 移動処理
	move = { move.x,player_->GetVelocity().y,move.z };
	player_->SetVelocity(move);

}

void InactiveState::Update()
{
}
