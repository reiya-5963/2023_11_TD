#include "FocusCamera.h"
#include "R_Math.h"
#include "ImGuiManager.h"
#include "Input.h"

void FocusCamera::Initialize()
{
	viewProjection.Initialize();
	// 調整項目クラスのインスタンス取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	// グループ名設定
	const char* groupName = "Camera";
	// 指定した名前でグループ追加
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "StartPosition", animater_.startPoint_);
	globalVariables->AddItem(groupName, "EndPosition", animater_.endPoint_);
	globalVariables->AddItem(groupName, "Frame",static_cast<int>(animater_.frame_));
	ApplyGlobalVariables();
	//animater_.frame_ = 120;

	const float xCenter = 48 / 2.0f - 0.5f;

	// 左下
	Vector2 squares = {12.0f,8.5f};

	Vector3 map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f + 1.0f), -40.0f };
	viewProjection.translation_ = map_CenterPoint;
	centers_.push_back(map_CenterPoint);

	// 右下
	squares = { 36.0f,8.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f + 1.0f), -40.0f };
	centers_.push_back(map_CenterPoint);

	// 左上
	squares = { 12.0f,22.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f + 1.0f), -40.0f };
	centers_.push_back(map_CenterPoint);

	// 右上
	squares = { 36.0f,22.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f + 1.0f), -40.0f };
	centers_.push_back(map_CenterPoint);

	cameraPoint_ = kLeftBottom;
}

void FocusCamera::Update()
{

#ifdef _DEBUG
	
	ImGui::Begin("FocusCamera");
	ImGui::SliderFloat3("position", &viewProjection.translation_.x, -100, 100);
	ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, 0.01f, -3.14f, 3.14f);
	ImGui::DragFloat3("stRotate", &startRot_.x, 0.01f, -3.14f, 3.14f);
	ImGui::DragFloat3("edRotate", &endRot_.x, 0.01f, -3.14f, 3.14f);

	if (ImGui::TreeNode("list")) {
		ImGui::DragFloat3("l0", &centers_[0].x);
		ImGui::DragFloat3("l1", &centers_[1].x);
		ImGui::DragFloat3("l2", &centers_[2].x);
		ImGui::DragFloat3("l3", &centers_[3].x);
		ImGui::TreePop();
	}

	ImGui::Text("t : %f", animater_.ease_t_);

	ImGui::End();

	if (Input::GetInstance()->TriggerKey(DIK_T)) {
		cameraPoint_ = kLeftBottom;
	}
	if (Input::GetInstance()->TriggerKey(DIK_Y)) {
		cameraPoint_ = kRightBottom;
	}
	if (Input::GetInstance()->TriggerKey(DIK_U)) {
		cameraPoint_ = kLeftTop;
	}
	if (Input::GetInstance()->TriggerKey(DIK_I)) {
		cameraPoint_ = kRightTop;
	}

#endif // _DEBUG


	ApplyGlobalVariables();

	if (isEase_) {

		animater_.ease_t_ += 1.0f / static_cast<float>(animater_.frame_);

		if (animater_.ease_t_ >= 1.0f) {
			isEase_ = false;
			animater_.ease_t_ = 1.0f;
		}

		viewProjection.translation_ = R_Math::EaseOutQuadF(animater_.ease_t_, centers_[0], centers_[1]);
		//viewProjection.translation_ = R_Math::lerp(animater_.ease_t_, animater_.startPoint_, animater_.endPoint_);
		//viewProjection.rotation_ = R_Math::lerp(animater_.ease_t_, startRot_, endRot_);

	}
	else {
		animater_.ease_t_ = 0;
	}

	if (target_) {
		Vector3 offset = { 0,0,-40.0f };
		Vector3 WorldPoint = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };
		viewProjection.translation_ = R_Math::Add(WorldPoint, offset);
	}

	//viewProjection.translation_ = centers_[this->cameraPoint_];

	viewProjection.UpdateMatrix();
}

void FocusCamera::ApplyGlobalVariables()
{
	// 調整項目クラスのインスタンス取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	// グループ名設定
	const char* groupName = "Camera";

	// 取得
	animater_.startPoint_ = globalVariables->GetVector3Value(groupName, "StartPosition");
	animater_.endPoint_ = globalVariables->GetVector3Value(groupName, "EndPosition");
	animater_.frame_ = static_cast<uint32_t>(globalVariables->GetIntValue(groupName, "Frame"));

}

void FocusCamera::SettingAnimation(const Vector3& start, const Vector3& end)
{

	animater_.startPoint_ = start;
	animater_.endPoint_ = end;

}
