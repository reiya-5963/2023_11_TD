#include "WallGimmick.h"
#include "ImGuiManager.h"
#include "Input.h"

void WallGimmick::Initialize(Model* model)
{
	model_ = model;
	worldTransform_.Initialize();
	collisionWorldTransform_.Initialize();
	ease_T_ = 0;
	isBeforeStartup = false;
}

void WallGimmick::Update()
{
	ImGui::Begin("Wall");
	ImGui::Text("IsAction : %d", isAction_);
	ImGui::Text("ease : %f", (ease_T_));
	ImGui::DragFloat3("begin", &beginPoint_.x, 0.01f, -100, 100);
	ImGui::DragFloat3("end", &EndPoint_.x, 0.01f, -100, 100);
	ImGui::End();

	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {

		StartSetting();

	}

	float maxFrame = 120.0f;

	if (isAction_) {
		ease_T_ += 1.0f / maxFrame;

		if (ease_T_ >= 1.0f) {
			ease_T_ = 1.0f;
			isAction_ = false;
		}
		
		if (!worldTransform_.parent_) {
			if (isBeforeStartup) {
				worldTransform_.translation_ = R_Math::EaseInQuadF(ease_T_, beginPoint_, EndPoint_);
			}
			else {
				worldTransform_.translation_ = R_Math::EaseOutQuadF(ease_T_, EndPoint_, beginPoint_);
			}
		}
	}

	// 初期座標・最終座標・現在座標

	this->worldTransform_.UpdateMatrix();
	collisionWorldTransform_ = worldTransform_;
	this->collisionWorldTransform_.UpdateMatrix();
}

void WallGimmick::Draw(const ViewProjection& viewProjection)
{

	model_->Draw(worldTransform_, viewProjection);

}

void WallGimmick::OnCollision(WorldTransform* worldTransform)
{
	worldTransform;
}
