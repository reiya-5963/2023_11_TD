#include "WallGimmick.h"
#include "ImGuiManager.h"
#include "Input.h"
#include "CollisionTypeIdDef.h"
void WallGimmick::Initialize(Model* model)
{
	model_ = model;
	worldTransform_.Initialize();
	SetRadius({ 1.0f, 1.0f, 1.0f });
	collisionWorldTransform_.Initialize();
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kMoveGimmick));
}

void WallGimmick::Update()
{
	ImGui::Begin("Wall");
	ImGui::Text("IsAction : %d", isAction_);
	ImGui::DragFloat3("begin", &beginPoint_.x, 0.01f, -100, 100);
	ImGui::DragFloat3("end", &EndPoint_.x, 0.01f, -100, 100);
	ImGui::End();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		if (isAction_) {
			isAction_ = false;
			ease_T_ = 0;
		}
		else {
			isAction_ = true;
			ease_T_ = 0;
		}
	}

	if (isAction_) {
		if (ease_T_ >= 1.0f) {
			ease_T_ = 1.0f;
		}
		else {
			ease_T_ += 1.0f / 300.0f;
		}
		worldTransform_.translation_ = R_Math::lerp(ease_T_, worldTransform_.translation_, EndPoint_);
	}
	else {
		if (ease_T_ >= 1.0f) {
			ease_T_ = 1.0f;
		}
		else {
			ease_T_ += 1.0f / 300.0f;
		}
		worldTransform_.translation_ = R_Math::lerp(ease_T_, worldTransform_.translation_, beginPoint_);
	}

	this->worldTransform_.UpdateMatrix();
	collisionWorldTransform_.translation_ = worldTransform_.translation_;
	this->collisionWorldTransform_.UpdateMatrix();
}

void WallGimmick::Draw(const ViewProjection& viewProjection)
{

	model_->Draw(worldTransform_, viewProjection);

}

void WallGimmick::OnCollision([[maybe_unused]] Collider* other) {
}
