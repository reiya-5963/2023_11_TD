#include "InputState.h"
#include "Input.h"
#include "R_Math.h"
#include "ImGuiManager.h"

void ActiveState::Update()
{
	Vector3 move = {};

	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
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
			const float speed = 3.0f;

			move = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed,0,0
			};
		}

		//---ジャンプ入力---//
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
			player_->SetBehaviorRequest(Player::Behavior::kJump);
		}

	}

	move = { move.x,player_->GetVelocity().y,move.z };
	player_->SetVelocity(move);

}

void ActiveState::MoveInputProcess()
{
}

void InactiveState::Update()
{
}
