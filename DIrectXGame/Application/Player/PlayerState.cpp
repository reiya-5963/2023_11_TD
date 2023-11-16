﻿#include "PlayerState.h"
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
			const float speed = 0.1f;

			move = {
				(float)joyState.Gamepad.sThumbLX / SHRT_MAX * speed,0,0
			};
		}
	}
	if (Input::GetInstance()->PushKey(DIK_A)) {
		float speed = -0.1f;
		move.x = speed;
	}
	else if (Input::GetInstance()->PushKey(DIK_D)) {
		float speed = 0.1f;
		move.x = speed;
	}
	player_->MoveUpdate(move);

}

void InactiveState::Update()
{
}
