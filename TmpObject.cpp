#include "TmpObject.h"
#include "TextureManager.h"
#include "Input.h"
#include "Mapchip.h"
#include "ImGuiManager.h"
void TmpObject::Initialize(const std::vector<Model*>& models) {
	BaseCharacter::Initialize(models);
}

void TmpObject::Update() {

	// 
	const float deltaTime = 0.03f;

	/// プレイヤーの処理(仮)
	Input* input_ = Input::GetInstance();

	// 念のため画面外いったらワープ
	if (objectWorldTransform_.translation_.y < -10.0f) {
		acceleration_.y = 0.0f;
		objectWorldTransform_.translation_.y = 10.0f;
	}

	// A,Dで横移動
	if (input_->PushKey(DIK_D)) {
		velocity_.x = 5.0f;
	}
	else if (input_->PushKey(DIK_A)) {
		velocity_.x = -5.0f;
	}
	else {
		velocity_.x = 0.0f;
	}
	if (input_->PushKey(DIK_LSHIFT)) {
		velocity_.x *= 1.6f;
	}

	// スぺ―スでジャンプ
	if (input_->PushKey(DIK_SPACE) && !isJump) {
		isJump = true;
		isGround_ = false;
		acceleration_.y += 15.0f;
	}

	// 地面にふれたらジャンプ状態をオフに
	if (isGround_) {
		isJump = false;
	}

	// 重力
	acceleration_.y -= 9.8f * deltaTime;

	/// プレイヤーの処理終わり
	BaseCharacter::Update();
}

void TmpObject::Draw(const ViewProjection& viewProjection) {
	BaseCharacter::Draw(viewProjection);
}
