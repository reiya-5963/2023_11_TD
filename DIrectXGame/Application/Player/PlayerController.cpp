#include "PlayerController.h"

PlayerController::PlayerController()
{

}

void PlayerController::Initialize()
{
	player1_ = std::make_unique<Player>();
	player2_ = std::make_unique<Player>();

	controlNum_ = ControlNum::kOne;

	Model* p_model;
	p_model = Model::CreateFlomObj("cube");

	std::vector<Model*> p_models = {
	p_model
	};

	Vector3 position = { 0,3,0 };
	player1_->Initialize(p_models);
	player1_->Setting(position, 0);

	position = { 2.5f,3,0 };
	player2_->Initialize(p_models);
	player2_->Setting(position, 0);

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

	if (isDelay_) {
		int kChangeFrame = 30;
		if (delayCount_++ >= kChangeFrame) {
			delayCount_ = 0;
			isDelay_ = false;
		}
	}

	if (Input::GetInstance()->GetJoyStickState(0, joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B && !isDelay_) {
			if (this->controlNum_ == ControlNum::kOne) {
				this->changeRequest_ = ControlNum::kTwo;
			}
			else {
				this->changeRequest_ = ControlNum::kOne;
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

//void PlayerController::SettingPlayer(Player* player, Vector3& position)
//{
//	player->Initialize(model_.get());
//	player->Setting(position, 0);
//}
