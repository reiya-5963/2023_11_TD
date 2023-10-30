#include "FollowCamera.h"
#include "R_Math.h"
#include "input/Input.h"

void FollowCamera::Initialize() {
	//ビュープロジェクションの初期化
	viewProjection_.Initialize();
}

void FollowCamera::Update() {

	// 現在のマウス位置を取得
	GetCursorPos(&mousePos_);

	// 初期位置から動かした距離を求める
	//xMouseDistance = float(mousePos_.x) - float(WinApp::kWindowWidth / 2);
	//yMouseDistance = float(mousePos_.y) - float(WinApp::kWindowHeight / 2);

	// 左右に視点を動かすなら
	//if (xMouseDistance != 0.0f) {
	//	viewProjection_.rotation_.y += xMouseDistance * move_mouseSpeed;
	//}
	// 上下に視点を動かすなら
	//if (yMouseDistance != 0.0f) {
	//	viewProjection_.rotation_.x += yMouseDistance * move_mouseSpeed;
	//}

	// カーソルを固定
	//SetCursorPos(int(WinApp::kWindowWidth / 2), int(WinApp::kWindowHeight / 2));

	// 視点を上、もしくは下に動かしたときの上限
	//if (viewProjection_.rotation_.x > 1.0f) {
	//	viewProjection_.rotation_.x = 1.0f;
	//}
	//if (viewProjection_.rotation_.x < -1.0f) {
	//	viewProjection_.rotation_.x = -1.0f;
	//}

	GetCursorPos(&mousePos_);

	HWND hwnd = WinApp::GetInstance()->GetHwnd();
	ScreenToClient(hwnd, &mousePos_);

	float speed = 0.003f;

	float mouseDistance = float(mousePos_.x) - float(preMousePos_.x);
	//float mouseDistancey = float(mousePos_.y) - float(preMousePos_.y);

	viewProjection_.rotation_.y += mouseDistance * speed;
	//viewProjection_.rotation_.x += mouseDistancey * speed;
	preMousePos_ = mousePos_;

	// もし追従対象がいれば
	if (target_) {
		// ターゲットとカメラの距離
		if (isFps_) {
			offset_ = fpsView_;
		}
		else if (!isFps_) {
			offset_ = tpsView_;
		}

		// 回転行列の生成
		Matrix4x4 rotateMat = R_Math::Multiply(
			R_Math::Multiply(
				R_Math::MakeRotateXMatrix(viewProjection_.rotation_.x),
				R_Math::MakeRotateYMatrix(viewProjection_.rotation_.y)),
			R_Math::MakeRotateZMatrix(viewProjection_.rotation_.z));

		// 回転行列でどの方向を見ても同じ距離になるよう調整
		offset_ = R_Math::TransformNormal(offset_, rotateMat);

		// ターゲットから調整した距離分を加算
		viewProjection_.translation_ = R_Math::Add(target_->translation_, offset_);
	}

	// ビュー行列の更新と転送
	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();

}


void FollowCamera::SetTarget(const WorldTransform* target) {
	// 追従対象を設定
	target_ = target;
}

ViewProjection& FollowCamera::GetViewProjection() {
	return viewProjection_;
}
