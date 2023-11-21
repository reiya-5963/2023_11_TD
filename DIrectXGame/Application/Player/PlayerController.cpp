#include "PlayerController.h"
#include "CollisionTypeIdDef.h"

PlayerController::PlayerController()
{

}

void PlayerController::Initialize()
{
	player1_ = std::make_unique<Player>();
	player2_ = std::make_unique<Player>();

	controlNum_ = ControlNum::kOne;

	Model* p_model1;
	p_model1 = Model::CreateFlomObj("cube");
	Model* p_model2;
	p_model2 = Model::CreateFlomObj("redBlock");

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

Collider* PlayerController::GetPlayer1()
{
	return player1_.get();
}

Collider* PlayerController::GetPlayer2()
{
	return player2_.get();
}

//void PlayerController::SettingPlayer(Player* player, Vector3& position)
//{
//	player->Initialize(model_.get());
//	player->Setting(position, 0);
//}
