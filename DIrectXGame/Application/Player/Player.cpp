#include "Player.h"
#include "Input.h"
#include "R_Math.h"
#include "ImGuiManager.h"
#include "CollisionTypeIdDef.h"

const Player::ConstAction Player::kConstAction_ = {
	60,30,300
};

void Player::Initialize(const std::vector<Model*>& models)
{
	// 初期化
	BaseCharacter::Initialize(models);
	worldTransformHat_.Initialize();
	worldTransformHat_.parent_ = &objectWorldTransform_;

	hatModel_.reset(Model::CreateFlomObj("hat"));
	worldTransformHat_.translation_.z = -0.3f;
}

void Player::Setting(const Vector3& position, uint32_t color)
{
	objectWorldTransform_.translation_ = position;
	color;
	behavior_ = Behavior::kRoot;
	SetRadius({ 2.0f, 2.0f, 2.0f });
	objectWorldTransform_.scale_ = GetRadius();

	info_.defaultRotate_ = 1.57f;
	info_.isLeft_ = false;

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
	ImGui::DragFloat3("velocity", &velocity_.x, 0.01f, -100, 100);
	ImGui::DragFloat3("hat", &worldTransformHat_.translation_.x, 0.01f, -100, 100);

	if (ImGui::TreeNode("ActionV")) {
		ImGui::Text("timer : %d", tmpValue_.timer_);
		ImGui::Text("const S : %d E : %d R : %d", kConstAction_.startTime_, kConstAction_.endTime_, kConstAction_.releaseTime_);
		ImGui::TreePop();
	}

	ImGui::End();

#endif // _DEBUG
	// プレイヤーの向き設定
	if (info_.isLeft_) {
		objectWorldTransform_.rotation_.y = -info_.defaultRotate_;
	}
	else {
		objectWorldTransform_.rotation_.y = info_.defaultRotate_;
	}


	if (request_) {

		behavior_ = request_.value();
		// 初期化
		switch (behavior_)
		{
		case Player::Behavior::kRoot:
			isMapChipCollision_ = true;
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

	// 更新
	switch (behavior_)
	{
	case Player::Behavior::kRoot:
		RootUpdate();
		break;
	case Player::Behavior::kJump:
		JumpUpdate();
		break;
	case Player::Behavior::kAction:
		ActionUpdate();
		break;
	}

	if (!isDriveObject_) {
		const float kGravity = 9.8f;

		const float deltaTime = 1.0f / 60.0f;

		Vector3 accelerationVector = { 0,-kGravity * deltaTime,0 };

		acceleration_ = R_Math::Add(acceleration_, accelerationVector);
	}

	if (behavior_ != Behavior::kAction) {
		BaseCharacter::Update();
	}
	worldTransformHat_.UpdateMatrix();
	ImGui::Text("%d", isGround_);
	acceleration_ = {};
}

void Player::Draw(const ViewProjection& viewProjection)
{
		
	BaseCharacter::Draw(viewProjection);

	if (typeid(*inputState_) == typeid(ActiveState)) {
		hatModel_->Draw(worldTransformHat_, viewProjection);
	}
}

void Player::OnCollision([[maybe_unused]] Collider* other) {

	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kMoveGimmick)) {
		const float kError = 0.2f;
		Vector3 direction = velocity_;
		R_Math::Normalize(direction);

		if (direction.y < 0 && direction.x < 0) {
			if (other->GetMax().x > GetMax().x) {
				velocity_.y = {};
				acceleration_.y = {};
				objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y;
				isDriveObject_ = true;

			}
			else if (other->GetMax().y > GetMax().y) {
				isDriveObject_ = false;
				objectWorldTransform_.parent_ = nullptr;
				velocity_.x = {};
				acceleration_.x = {};
				objectWorldTransform_.translation_.x = other->GetMax().x + GetRadius().x;
			}
			else {
				if (other->GetMax().x - GetMin().x > other->GetMax().y - GetMin().y) {
					velocity_.y = {};
					acceleration_.y = {};
					objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y;
					isDriveObject_ = true;
				}
				else if (other->GetMax().x - GetMin().x <= other->GetMax().y - GetMin().y) {
					isDriveObject_ = false;
					objectWorldTransform_.parent_ = nullptr;
					velocity_.x = {};
					acceleration_.x = {};
					objectWorldTransform_.translation_.x = other->GetMax().x + GetRadius().x;
				}
			}
			ImGui::Text("%d :left down", typeID_);
		}
		else if (direction.y < 0 && direction.x > 0) {
			if (other->GetMin().x < GetMin().x) {
				velocity_.y = {};
				acceleration_.y = {};
				objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y;
				isDriveObject_ = true;
			}
			else if (other->GetMax().y > GetMax().y) {
				isDriveObject_ = false;
				objectWorldTransform_.parent_ = nullptr;
				velocity_.x = {};
				acceleration_.x = {};
				objectWorldTransform_.translation_.x = other->GetMin().x - GetRadius().x;
			}
			else {
				if (GetMax().x - other->GetMin().x > other->GetMax().y - GetMin().y) {
					velocity_.y = {};
					acceleration_.y = {};
					objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y;
					isDriveObject_ = true;
				}
				else if (GetMax().x - other->GetMin().x <= other->GetMax().y - GetMin().y) {
					isDriveObject_ = false;
					objectWorldTransform_.parent_ = nullptr;
					velocity_.x = {};
					acceleration_.x = {};
					objectWorldTransform_.translation_.x = other->GetMin().x - GetRadius().x;
				}
			}
			ImGui::Text("%d :right down", typeID_);
		}
		else if (direction.y > 0 && direction.x < 0) {
			if (other->GetMax().x > GetMax().x) {
				velocity_.y = {};
				acceleration_.y = {};
				isDriveObject_ = true;
				objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
			}
			else if (other->GetMin().y < GetMin().y) {
				objectWorldTransform_.parent_ = nullptr;
				velocity_.x = {};
				acceleration_.x = {};
				isDriveObject_ = false;
				objectWorldTransform_.translation_.x = other->GetMax().x + GetRadius().x;
			}
			else {
				if (other->GetMax().x - GetMin().x > GetMax().y - other->GetMin().y) {
					velocity_.y = {};
					isDriveObject_ = false;
					acceleration_.y = {};
					objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
				}
				else if (other->GetMax().x - GetMin().x <= GetMax().y - other->GetMin().y) {
					objectWorldTransform_.parent_ = nullptr;
					velocity_.x = {};
					isDriveObject_ = false;
					acceleration_.x = {};
					objectWorldTransform_.translation_.x = other->GetMax().x + GetRadius().x;
				}
			}
			ImGui::Text("%d :left up", typeID_);
		}
		else if (direction.y > 0 && direction.x > 0) {
			if (other->GetMin().x < GetMin().x) {
				velocity_.y = {};
				acceleration_.y = {};
				isDriveObject_ = true;
				objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
			}
			else if (other->GetMin().y < GetMin().y) {
				objectWorldTransform_.parent_ = nullptr;
				velocity_.x = {};
				acceleration_.x = {};
				isDriveObject_ = false;
				objectWorldTransform_.translation_.x = other->GetMin().x - GetRadius().x;
			}
			else {
				if (other->GetMin().x - GetMax().x > GetMax().y - other->GetMin().y) {
					velocity_.y = {};
					acceleration_.y = {};
					isDriveObject_ = true;
					objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
				}
				else if (other->GetMin().x - GetMax().x <= GetMax().y - other->GetMin().y) {
					objectWorldTransform_.parent_ = nullptr;
					velocity_.x = {};
					acceleration_.x = {};
					isDriveObject_ = false;
					objectWorldTransform_.translation_.x = other->GetMin().x - GetRadius().x;
				}
			}
			ImGui::Text("%d :right up", typeID_);
		}
		else if (direction.y < 0) {
			if ((GetMax().x > other->GetMin().x + kError && GetMin().x < other->GetMax().x) || (GetMin().x < other->GetMax().x - kError && GetMax().x > other->GetMin().x)) {
				velocity_.y = {};
				acceleration_.y = {};
				objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y;
				isDriveObject_ = true;
				ImGui::Text("%d : down", typeID_);
			}

		}
		else if (direction.y > 0) {
			if ((GetMax().x > other->GetMin().x + kError && GetMin().x < other->GetMax().x) || (GetMin().x < other->GetMax().x - kError && GetMax().x > other->GetMin().x)) {
				velocity_.y = {};
				acceleration_.y = {};
				objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
				isDriveObject_ = false;

				ImGui::Text("%d : up", typeID_);
			}

		}
		if (isDriveObject_ && objectWorldTransform_.parent_ == nullptr) {
			Vector3 selfWorldPos = { objectWorldTransform_.matWorld_.m[3][0], objectWorldTransform_.matWorld_.m[3][1],objectWorldTransform_.matWorld_.m[3][2] };

			objectWorldTransform_.translation_ = selfWorldPos - other->GetWorldPosition();
			objectWorldTransform_.parent_ = other->GetParent();
			//isMapChipCollision_ = false;
		}
	}

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
	if (typeid(*inputState_) == typeid(InactiveState)) {
		velocity_ = {};
	}
}

void Player::JumpInitialize()
{
	isDriveObject_ = false;
	isGround_ = false;
	if (objectWorldTransform_.parent_) {
		Vector3 parentWorldPos = { objectWorldTransform_.parent_->matWorld_.m[3][0], objectWorldTransform_.parent_->matWorld_.m[3][1] ,objectWorldTransform_.parent_->matWorld_.m[3][2] };
		objectWorldTransform_.translation_ = objectWorldTransform_.translation_ + parentWorldPos;
		objectWorldTransform_.parent_ = nullptr;
		objectWorldTransform_.UpdateMatrix();
		//isMapChipCollision_ = true;

	}
	const float kFirstSpeed = 15.0f;

	velocity_.y = kFirstSpeed;

	isMapChipCollision_ = true;
}

void Player::JumpUpdate()
{
	if (isGround_ || isDriveObject_) {
		request_ = Behavior::kRoot;
		if (typeid(*inputState_) == typeid(InactiveState)) {
			velocity_ = {};
		}
	}
}

void Player::ActionInitialize()
{
	objectWorldTransform_.parent_ = nullptr;
	isMapChipCollision_ = false;
	actionState_ = ActionState::kReserve;
	tmpValue_.timer_ = 0;
}

void Player::ActionUpdate()
{
	objectWorldTransform_.parent_ = nullptr;
	velocity_ = {};
	tmpValue_.timer_++;
	switch (actionState_)
	{
	// 憑りつくまでの動き
	case Player::ActionState::kReserve:
		if (tmpValue_.timer_ > kConstAction_.startTime_) {
			actionState_ = ActionState::kNow;
			tmpValue_.timer_ = 0;
			tmpValue_.ease_t_ = 0;
		}
		else {
			tmpValue_.ease_t_ += 1.0f / (float)kConstAction_.startTime_;
			if (tmpValue_.ease_t_ >= 1.0f) {
				tmpValue_.ease_t_ = 1.0f;
			}
			Vector3 WorldPosition = R_Math::lerp(tmpValue_.ease_t_, tmpValue_.startPoint_, tmpValue_.endPoint_);
			objectWorldTransform_.translation_ = WorldPosition;
		}

		if (tmpValue_.startPoint_.x > tmpValue_.endPoint_.x) {
			info_.isLeft_ = true;
		}
		else {
			info_.isLeft_ = false;
		}

		break;
	// 憑りつき中
	case Player::ActionState::kNow:
		if (iGimmickPtr_ != nullptr && tmpValue_.timer_ < 10) {
			iGimmickPtr_->StartSetting(float(kConstAction_.releaseTime_) / 2.0f);
			//iGimmickPtr_ = nullptr;
		}

		if (tmpValue_.timer_ > kConstAction_.releaseTime_) {
			actionState_ = ActionState::kRelease;
			tmpValue_.timer_ = 0;
		}

		break;
	// 解除中
	case Player::ActionState::kRelease:
		if (iGimmickPtr_ != nullptr && tmpValue_.timer_ < 10) {
			iGimmickPtr_->ReturnSetting(float(kConstAction_.endTime_));
			iGimmickPtr_ = nullptr;
		}
		if (tmpValue_.timer_ > kConstAction_.endTime_) {
			tmpValue_.timer_ = 0;
			behavior_ = Behavior::kRoot;
		}
		else {
			tmpValue_.ease_t_ += 1.0f / (float)kConstAction_.endTime_;
			if (tmpValue_.ease_t_ >= 1.0f) {
				tmpValue_.ease_t_ = 1.0f;
			}
			Vector3 WorldPosition = R_Math::lerp(tmpValue_.ease_t_, tmpValue_.endPoint_, tmpValue_.startPoint_);
			objectWorldTransform_.translation_ = WorldPosition;
		}

		if (tmpValue_.startPoint_.x > tmpValue_.endPoint_.x) {
			info_.isLeft_ = false;
		}
		else {
			info_.isLeft_ = true;
		}

		break;
	}

	objectWorldTransform_.UpdateMatrix();

}

void Player::GhostSetting()
{
	// 移動用の座標設定
	tmpValue_ = {};
	tmpValue_.startPoint_ = GetWorldPosition();
	tmpValue_.endPoint_ = iGimmickPtr_->GetWorldPosition();
	
}
