#include "FocusCamera.h"
#include "R_Math.h"
#include "ImGuiManager.h"

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
}

void FocusCamera::Update()
{

#ifdef _DEBUG
	
	ImGui::Begin("FocusCamera");
	ImGui::SliderFloat3("position", &viewProjection.translation_.x, -100, 100);
	ImGui::DragFloat3("rotate", &viewProjection.rotation_.x, 0.01f, -3.14f, 3.14f);
	ImGui::DragFloat3("stRotate", &startRot_.x, 0.01f, -3.14f, 3.14f);
	ImGui::DragFloat3("edRotate", &endRot_.x, 0.01f, -3.14f, 3.14f);

	ImGui::End();

#endif // _DEBUG

	ApplyGlobalVariables();

	if (isEase_) {

		animater_.ease_t_ += 1.0f / static_cast<float>(animater_.frame_);

		if (animater_.ease_t_ >= 1.0f) {
			isEase_ = false;
			animater_.ease_t_ = 1.0f;
		}

		viewProjection.translation_ = R_Math::lerp(animater_.ease_t_, animater_.startPoint_, animater_.endPoint_);
		viewProjection.rotation_ = R_Math::lerp(animater_.ease_t_, startRot_, endRot_);

	}
	else {
		animater_.ease_t_ = 0;
	}

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
