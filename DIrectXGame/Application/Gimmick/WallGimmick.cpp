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
	ease_T_ = 0;
	isBeforeStartup = false;
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kMoveGimmick));
}

void WallGimmick::Update()
{
#ifdef _DEBUG
	ImGui::Begin("Wall");
	ImGui::Text("IsAction : %d", isAction_);
	ImGui::Text("ease : %f", (ease_T_));
	ImGui::Text("maxF : %f", maxFrame_);
	ImGui::DragFloat3("begin", &beginPoint_.x, 0.01f, -100, 100);
	ImGui::DragFloat3("end", &EndPoint_.x, 0.01f, -100, 100);
	ImGui::End();
#endif // _DEBUG

	if (isAction_) {
		ease_T_ += 1.0f / maxFrame_;

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
				if (ease_T_ >= 1.0f) {
					isSetUp_ = false;
				}
			}
		}
	}

	// 初期座標・最終座標・現在座標

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
