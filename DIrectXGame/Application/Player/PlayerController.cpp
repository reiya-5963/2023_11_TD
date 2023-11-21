#include "PlayerController.h"
#include <limits>

// マクロの停止
#undef max

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

	if (Input::GetInstance()->TriggerKey(DIK_8)) {
		float length = std::numeric_limits<float>::max();
		IGimmick* tmpGimmick = nullptr;
		Vector3 playerPosition = {};
		if (typeid(*player1_.get()) == typeid(ActiveState)) {
			playerPosition = player1_->GetWorldPosition();
		}
		else {
			playerPosition = player2_->GetWorldPosition();
		}

		float radius = 30.0f;

		Vector2_AABB player = {
			{playerPosition.x - radius,playerPosition.y - radius},
			{playerPosition.x + radius,playerPosition.y + radius},
		};

		for (IGimmick* gimmick : gimmickManager_->GetGimmickList()) {
			// 親を所持している場合スキップ
			if (gimmick->GetWorldTransform()->parent_ != nullptr) {
				continue;
			}
			// 長さ用
			Vector3 range = R_Math::Subtract(playerPosition, gimmick->GetWorldPosition());
			// オブジェクトのAABB
			Vector2_AABB gim = {
				{gimmick->GetWorldPosition().x - radius, gimmick->GetWorldPosition().y - radius},
				{gimmick->GetWorldPosition().x + radius, gimmick->GetWorldPosition().y + radius},
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
		if (tmpGimmick != nullptr) {
			tmpGimmick->SetIsAction(true);
		}

	}

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

bool PlayerController::IsAABBCollision(const Vector2_AABB& aabb1, const Vector2_AABB& aabb2) {
	if ((aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) &&	// X軸
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y))	// Y軸
	{	// Z軸
		return true;
	}
	return false;
}
