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

	const float xCenter = 72 / 2.0f - 0.5f;

	// 左下
	Vector2 squares = {12.0f,8.5f};

	Vector3 map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
	viewProjection.translation_ = map_CenterPoint;
	centers_.push_back(map_CenterPoint);

	// 中下
	squares = { 36.0f,8.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
	centers_.push_back(map_CenterPoint);

	// 右下
	squares = { 60.0f,8.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
	centers_.push_back(map_CenterPoint);
	
	// 左上
	squares = { 12.0f, 22.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
	centers_.push_back(map_CenterPoint);

	// 中上
	squares = { 36.0f, 22.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
	centers_.push_back(map_CenterPoint);

	// 右上
	squares = { 60.0f, 22.5f };
	map_CenterPoint = { ((squares.x - 1.0f) - xCenter) * 2.0f, (squares.y * 2.0f) - 0.5f, -38.0f };
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
		ImGui::DragFloat3("l3", &centers_[4].x);
		ImGui::DragFloat3("l3", &centers_[5].x);
		ImGui::TreePop();
	}

	ImGui::Text("t : %f", animater_.ease_t_);
	ImGui::Text("cameraPoint : %d", cameraPoint_);
	ImGui::Text("isMoveCam : %d", isMoveMap_);
	ImGui::Text("isEase : %d", isEase_);
	ImGui::Text("isPlayer1 : %d", isControlPlayer1);
	ImGui::Text("isprePlayer1 : %d", isPreControlPlayer1);
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

#endif // _DEBUG^


	//if (!isEase_ && isControlPlayer1) {
	//	if (viewProjection.translation_ == centers_[kLeftBottom]) {
	//		if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
	//			player1ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.x > float(WinApp::kWindowWidth) ) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kRightBottom]) {
	//		if (player1ScreenPos_.x < 0.0f &&
	//			player1ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.x < 0.0f) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kLeftTop]) {
	//		if (player1ScreenPos_.x > float(WinApp::kWindowWidth) &&
	//			player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.x > float(WinApp::kWindowWidth)) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kRightTop]) {
	//		if (player1ScreenPos_.x < 0.0f &&
	//			player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.x < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//		else if (player1ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//	}
	//}
	//else if (!isEase_ && !isControlPlayer1) {
	//	if (viewProjection.translation_ == centers_[kLeftBottom]) {
	//		if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
	//			player2ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kRightBottom]) {
	//		if (player2ScreenPos_.x < 0.0f &&
	//			player2ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.x < 0.0f) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.y < 0.0f) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kLeftTop]) {
	//		if (player2ScreenPos_.x > float(WinApp::kWindowWidth) &&
	//			player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.x > float(WinApp::kWindowWidth)) {
	//			cameraPoint_ = kRightTop;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//	}
	//	else if (viewProjection.translation_ == centers_[kRightTop]) {
	//		if (player2ScreenPos_.x < 0.0f &&
	//			player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kLeftBottom;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.x < 0.0f) {
	//			cameraPoint_ = kLeftTop;
	//			isEase_ = true;
	//		}
	//		else if (player2ScreenPos_.y > float(WinApp::kWindowHeight)) {
	//			cameraPoint_ = kRightBottom;
	//			isEase_ = true;
	//		}
	//	}
	//}

	// もし前フレームで操作プレイヤーが違うなら
	//if (isPreControlPlayer1 != isControlPlayer1 && !isEase_) {
	//	isEase_ = true;
	//}

	ApplyGlobalVariables();

	if (isEase_) {

		//animater_.ease_t_ += 1.0f / static_cast<float>(animater_.frame_);
		animater_.ease_t_ = 1.0f;
		if (animater_.ease_t_ >= 1.0f) {
			isEase_ = false;
			isMoveMap_ = false;
			animater_.ease_t_ = 1.0f;
		}

		int i = 0;
		if (cameraPoint_ == kLeftBottom) {
			i = 0;
		}
		else if (cameraPoint_ == kMiddleBottom) {
			i = 1;
		}
		else if (cameraPoint_ == kRightBottom) {
			i = 2;
		}
		else if (cameraPoint_ == kLeftTop) {
			i = 3;
		}
		else if (cameraPoint_ == kMiddleTop) {
			i = 4;
		}
		else if (cameraPoint_ == kRightTop) {
			i = 5;
		}

		viewProjection.translation_ = R_Math::EaseOutQuadF(animater_.ease_t_, viewProjection.translation_, centers_[i]);
	}
	else {
		animater_.ease_t_ = 0;
	}

	if (target_) {
		Vector3 offset = { 0,0,-40.0f };
		Vector3 WorldPoint = { target_->matWorld_.m[3][0],target_->matWorld_.m[3][1],target_->matWorld_.m[3][2] };
		viewProjection.translation_ = R_Math::Add(WorldPoint, offset);
	}

	viewProjection.UpdateMatrix();
	isPreControlPlayer1 = isControlPlayer1;
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

void FocusCamera::SetPlayerWorld2ScreenPos(Vector3& worldPos1, Vector3& worldPos2) {
	Matrix4x4 matViewPort =
		R_Math::MakeViewPortMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	Matrix4x4 matViewProjectionViewPort = R_Math::Multiply(
		viewProjection.matView, R_Math::Multiply(viewProjection.matProjection, matViewPort));

	Vector3 tmpPlayer1Pos = R_Math::TransformCoord(worldPos1, matViewProjectionViewPort);

	player1ScreenPos_ = { tmpPlayer1Pos.x, tmpPlayer1Pos.y };

	Vector3 tmpPlayer2Pos = R_Math::TransformCoord(worldPos2, matViewProjectionViewPort);

	player2ScreenPos_ = { tmpPlayer2Pos.x, tmpPlayer2Pos.y };


}

