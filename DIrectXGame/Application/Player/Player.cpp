#include "Player.h"
#include "Input.h"
#include "R_Math.h"
#include "ImGuiManager.h"

void Player::Initialize(const std::vector<Model*>& models)
{
	// 初期化
	BaseCharacter::Initialize(models);

}

void Player::Setting(const Vector3& position, uint32_t color)
{
	objectWorldTransform_.translation_ = position;
	color;
	behavior_ = Behavior::kRoot;
	objectWorldTransform_.scale_ = { 5,5,1 };
}

void Player::Update()
{

#ifdef _DEBUG

	ImGui::Begin("Player");
	if (ImGui::TreeNode("worldTransform")) {
		float maxValue = 500.0f;
		ImGui::DragFloat3("position", &objectWorldTransform_.translation_.x, 0.01f, -maxValue, maxValue);
		ImGui::DragFloat3("rotate", &objectWorldTransform_.rotation_.x, 0.01f, -maxValue, maxValue);
		ImGui::DragFloat3("scale", &objectWorldTransform_.scale_.x, 0.01f, -maxValue, maxValue);
		ImGui::TreePop();
	}
	ImGui::Text("behavior : %d", static_cast<int>(behavior_));
	ImGui::Text("test : %d", test);
	ImGui::DragFloat3("velocity", &velocity_.x, 0.01f, -100, 100);
	ImGui::End();

#endif // _DEBUG


	if (request_) {

		behavior_ = request_.value();
		// 初期化
		switch (behavior_)
		{
		case Player::Behavior::kRoot:
			//RootInitialize();
			break;
		case Player::Behavior::kJump:
			JumpInitialize();
			break;
		case Player::Behavior::kAction:
			ActionInitialize();
			break;
		}

		request_ = std::nullopt;
	}
	inputState_->Update();

	switch (behavior_)
	{
	case Player::Behavior::kRoot:
		//RootUpdate();
		break;
	case Player::Behavior::kJump:
		JumpUpdate();
		break;
	case Player::Behavior::kAction:
		ActionUpdate();
		break;
	}

	//objectWorldTransform_.translation_ = R_Math::Add(objectWorldTransform_.translation_, velocity_);

	BaseCharacter::Update();

	collisionWorldTransform_ = objectWorldTransform_;
	collisionWorldTransform_.UpdateMatrix();

}

void Player::Draw(const ViewProjection& viewProjection)
{
	if (typeid(*inputState_) == typeid(ActiveState)) {
		BaseCharacter::Draw(viewProjection);
	}
}

void Player::OnCollision(WorldTransform* worldTransform)
{
	worldTransform;
	//request_ = Behavior::kRoot;
}


void Player::MoveUpdate(Vector3& moveDirect)
{
	objectWorldTransform_.translation_ = R_Math::Add(objectWorldTransform_.translation_, moveDirect);
}

void Player::SetState(InputState* state)
{
	inputState_ = state;
	inputState_->SetPlayer(this);
}

void Player::RootInitialize()
{
	velocity_.y = 0;
}

void Player::RootUpdate()
{

	inputState_->Update();

}

void Player::JumpInitialize()
{
	isGround_ = false;

	const float kFirstSpeed = 5.0f;
	
	velocity_.y = kFirstSpeed;

}

void Player::JumpUpdate()
{
	const float kGravity = 9.8f;

	const float deltaTime = 1.0f / 60.0f;

	Vector3 accelerationVector = { 0,-kGravity * deltaTime,0 };

	velocity_ = R_Math::Add(velocity_, accelerationVector);
	test += 1;

	if (isGround_) {
		request_ = Behavior::kRoot;
		if (typeid(*inputState_) == typeid(InactiveState)) {
			velocity_ = {};
		}
	}


}

void Player::ActionInitialize()
{

}

void Player::ActionUpdate()
{

}
