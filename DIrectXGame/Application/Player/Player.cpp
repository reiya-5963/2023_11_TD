#include "Player.h"
#include "Input.h"
#include "R_Math.h"
#include "ImGuiManager.h"
#include "CollisionTypeIdDef.h"

const Player::ConstAction Player::kConstAction_ = {
	30,30,150
};
const float Player::kFisrtJumpPower_ = 45.0f;

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
	SetRadius({ 1.0f, 1.0f, 1.0f });
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
	if (isGround_) {
		isDriveObject_ = true;
	}
	else if (objectWorldTransform_.parent_) {
		isDriveObject_ = true;
	}
	else if (objectWorldTransform_.parent_ == nullptr) {
		isDriveObject_ = false;
	}

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
			isDriveObject_ = false;
			actionState_ = ActionState::kReserve;
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

	if (!isDriveObject_ || !isCollisionObject_) {
		const float kGravity = 9.8f;

		const float deltaTime = 10.0f / 60.0f;

		Vector3 accelerationVector = { 0,-kGravity * deltaTime,0 };

		acceleration_ = R_Math::Add(acceleration_, accelerationVector);
	}

	if (behavior_ != Behavior::kAction) {
		BaseCharacter::Update();
	}
	worldTransformHat_.UpdateMatrix();
	ImGui::Text("isDrive : %d", isDriveObject_);


	if (isCollisionObject_ && isPreCollisionObject && isDriveObject_) {
		if (objectWorldTransform_.parent_) {
			Vector3 parentPos = { objectWorldTransform_.parent_->matWorld_.m[3][0], objectWorldTransform_.parent_->matWorld_.m[3][1], objectWorldTransform_.parent_->matWorld_.m[3][2] };
			objectWorldTransform_.parent_ = nullptr;
			objectWorldTransform_.translation_ += parentPos;
			objectWorldTransform_.UpdateMatrix();
		}
	}

	isPreCollisionObject = isCollisionObject_;

	isCollisionObject_ = false;
	acceleration_ = {};

}

void Player::Draw(const ViewProjection& viewProjection)
{

	if (actionState_ != ActionState::kNow && actionState_ != ActionState::kRelease) {
		BaseCharacter::Draw(viewProjection);

		if (typeid(*inputState_) == typeid(ActiveState)) {
			hatModel_->Draw(worldTransformHat_, viewProjection);
		}
	}
}

void Player::OnCollision([[maybe_unused]] Collider* other) {



	ImGui::Text("isDrive : %d", isDriveObject_);
	ImGui::Text("my : %f, %f", GetWorldPosition().x, GetWorldPosition().y);
	ImGui::Text("other : %f, %f", other->GetWorldPosition().x, other->GetWorldPosition().y);

	if (behavior_ == Behavior::kAction) {
		return;
	}
	
	Vector3 myWorldPos = { objectWorldTransform_.matWorld_.m[3][0], objectWorldTransform_.matWorld_.m[3][1],objectWorldTransform_.matWorld_.m[3][2] };
	Vector3 beforePos = myWorldPos - velocity_;
	Vector3 otherWorldPos = other->GetWorldPosition();
	Vector3 direction = otherWorldPos - myWorldPos;


	if (other->GetTypeID() == static_cast<uint32_t>(CollisionTypeIdDef::kMoveGimmick)) {
		R_Math::Normalize(direction);

		if (objectWorldTransform_.parent_) {
			if ((GetMin().x <= other->GetMax().x && GetMax().x >= other->GetMin().x) &&
				(GetMin().y <= other->GetMax().y && GetMax().y >= other->GetMin().y) &&
				(GetMin().z <= other->GetMax().z && GetMax().z >= other->GetMin().z)) {
				objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y - otherWorldPos.y;
			}
			
		}
		else if ((GetMin().x <= other->GetMax().x && GetMax().x >= other->GetMin().x) &&
			(GetMin().y <= other->GetMax().y && GetMax().y >= other->GetMin().y) &&
			(GetMin().z <= other->GetMax().z && GetMax().z >= other->GetMin().z)) {

			if (std::abs(direction.x) > std::abs(direction.y)) {
				
				if (direction.x < 0) {
					isDriveObject_ = false;
					objectWorldTransform_.translation_.x = other->GetMax().x + GetRadius().x;

				}
				else if (direction.x > 0) {
					isDriveObject_ = false;
					objectWorldTransform_.translation_.x = other->GetMin().x - GetRadius().x;

				}
				velocity_.x = {};
				acceleration_.x = {};

			}
			else if (std::abs(direction.x) < std::abs(direction.y) - 0.17f) {
				if (direction.y < 0) {
					isDriveObject_ = true;
					objectWorldTransform_.translation_.y = other->GetMax().y + GetRadius().y - velocity_.y;

				}
				else if (direction.y > 0) {
					isDriveObject_ = false;
					objectWorldTransform_.translation_.y = other->GetMin().y - GetRadius().y;
					velocity_.y = {};
					acceleration_.y = {};

				}
			}

			if (isDriveObject_) {
				Vector3 selfWorldPos = { objectWorldTransform_.matWorld_.m[3][0], objectWorldTransform_.matWorld_.m[3][1],objectWorldTransform_.matWorld_.m[3][2] };

				objectWorldTransform_.translation_ = selfWorldPos - other->GetWorldPosition();
				objectWorldTransform_.parent_ = other->GetParent();

			}
		}
		isCollisionObject_ = true;

	}
	objectWorldTransform_.UpdateMatrix();

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
	// アクティブかどうか
	if (typeid(*inputState_) == typeid(InactiveState)) {
		//velocity_ = {};
		velocity_.x = 0;
		velocity_.z = 0;
	}
}

void Player::JumpInitialize()
{
	isDriveObject_ = false;
	isGround_ = false;
	// 親子関係の際
	if (objectWorldTransform_.parent_) {
		Vector3 parentWorldPos = { objectWorldTransform_.parent_->matWorld_.m[3][0], objectWorldTransform_.parent_->matWorld_.m[3][1] ,objectWorldTransform_.parent_->matWorld_.m[3][2] };
		objectWorldTransform_.translation_ = objectWorldTransform_.translation_ + parentWorldPos;
		objectWorldTransform_.parent_ = nullptr;
		objectWorldTransform_.UpdateMatrix();
		//isMapChipCollision_ = true;

	}
	//const float kFirstSpeed = 30.0f;

	velocity_.y = kFisrtJumpPower_;

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
	// 衝突処理避け用
	objectWorldTransform_.parent_ = nullptr;
	isMapChipCollision_ = false;
	// ステート選択
	actionState_ = ActionState::kReserve;
	tmpValue_.timer_ = 0;
}

void Player::ActionUpdate()
{
	// 毎フレーム更新
	objectWorldTransform_.parent_ = nullptr;
	velocity_ = {};
	tmpValue_.timer_++;
	// ステートに合わせた処理
	switch (actionState_)
	{
		// 憑りつくまでの動き
	case Player::ActionState::kReserve:
		// 終了
		if (tmpValue_.timer_ > kConstAction_.startTime_) {
			actionState_ = ActionState::kNow;
			tmpValue_.timer_ = 0;
			tmpValue_.ease_t_ = 0;
		}
		// 移動中
		else {
			tmpValue_.ease_t_ += 1.0f / (float)kConstAction_.startTime_;
			if (tmpValue_.ease_t_ >= 1.0f) {
				tmpValue_.ease_t_ = 1.0f;
			}
			Vector3 WorldPosition = R_Math::lerp(tmpValue_.ease_t_, tmpValue_.startPoint_, tmpValue_.endPoint_);
			objectWorldTransform_.translation_ = WorldPosition;
		}
		// 向きの設定
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
		}

		if (tmpValue_.timer_ > kConstAction_.releaseTime_) {
			actionState_ = ActionState::kRelease;
			tmpValue_.timer_ = 0;
		}

		break;
		// 解除中
	case Player::ActionState::kRelease:
		// 開始
		if (iGimmickPtr_ != nullptr && tmpValue_.timer_ < 10) {
			iGimmickPtr_->ReturnSetting(float(kConstAction_.endTime_));
			iGimmickPtr_ = nullptr;
		}
		// 終了
		if (tmpValue_.timer_ > kConstAction_.endTime_) {
			tmpValue_.timer_ = 0;
			request_ = Behavior::kRoot;
		}
		// 移動中
		else {
			tmpValue_.ease_t_ += 1.0f / (float)kConstAction_.endTime_;
			if (tmpValue_.ease_t_ >= 1.0f) {
				tmpValue_.ease_t_ = 1.0f;
			}
			Vector3 WorldPosition = R_Math::lerp(tmpValue_.ease_t_, tmpValue_.endPoint_, tmpValue_.startPoint_);
			objectWorldTransform_.translation_ = WorldPosition;
		}
		// プレイヤーの向きの設定
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
