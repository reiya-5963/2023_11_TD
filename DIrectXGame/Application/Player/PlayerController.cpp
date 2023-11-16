#include "PlayerController.h"

PlayerController::PlayerController()
{
	model_.reset(Model::CreateFlomObj("cube"));
}

void PlayerController::Initialize()
{
	player1_ = std::make_unique<Player>();
	player2_ = std::make_unique<Player>();

	controlNum_ = ControlNum::kOne;

	Vector3 position = { 0,0,0 };
	SettingPlayer(player1_.get(), position);
	position = { 2.5f,0,0 };
	SettingPlayer(player2_.get(), position);

	player1_->SetState(new ActiveState());
	player2_->SetState(new InactiveState());

}

void PlayerController::Update()
{
	ChangeControl();
	player1_->Update();
	player2_->Update();
}

void PlayerController::Draw(ViewProjection& viewprojection)
{
	player1_->Draw(viewprojection);
	player2_->Draw(viewprojection);
}

void PlayerController::ChangeControl()
{
	XINPUT_STATE joyState;
	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
			player1_->SetState(new InactiveState());
			player2_->SetState(new ActiveState());
		}
	}

}

void PlayerController::SettingPlayer(Player* player, Vector3& position)
{
	player->Initialize(model_.get());
	player->Setting(position, 0);
}
