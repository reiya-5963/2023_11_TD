#include "TmpObject.h"
#include "TextureManager.h"
#include "Input.h"
#include "Mapchip.h"
#include "ImGuiManager.h"
void TmpObject::Initialize() {
	if (!worldTrans_) {
		worldTrans_ = std::make_unique<WorldTransform>();
		worldTrans_->Initialize();
	}
	model_.reset(Model::CreateFlomObj("cube"));
	worldTrans_->translation_.y = 10.0f;
	worldTrans_->UpdateMatrix();

	//beforePos = worldTrans_->translation_;
	//afterPos = worldTrans_->translation_;

	velocity_ = {};
	acceleration_ = {};
}

void TmpObject::Update() {


	const float deltaTime = 0.03f;

	/// プレイヤーの処理
	Input* input_ = Input::GetInstance();

	// 念のため画面外いったらワープ
	if (worldTrans_->translation_.y < -10.0f) {
		acceleration_.y = 0.0f;
		worldTrans_->translation_.y = 10.0f;
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


#pragma region 共通の処理

	// 動く前の位置と、動いた後の位置を取得
	Vector3 beforePos = worldTrans_->translation_;
	Vector3& afterPos = worldTrans_->translation_;

	// 速度に加速度を加算
	velocity_ += acceleration_;
	Vector3 fixVelocity = velocity_ * deltaTime;

	// 速度を調整　速度を調整　速度を調整　速度を調整　速度を調整　速度を調整　速度を調整　速度を調整　速度を調整　速度を調整 //
	if (fixVelocity.y <= -1.0f) {
		fixVelocity.y = -1.0f;
	}
	else if (fixVelocity.y >= 1.0f) {
		fixVelocity.y = 1.0f;
	}
	if (fixVelocity.x <= -2.0f) {
		fixVelocity.x = -2.0f;
	}
	else if (fixVelocity.x >= 2.0f) {
		fixVelocity.x = 2.0f;
	}

	// 速度を調整し、位置に加算
	worldTrans_->translation_ += fixVelocity;

	// 一旦地面判定を無効に
	isGround_ = false;

	// もしマップチップ当たり判定をとるなら
	if(isMapChipCollision_) {
		// beforeからafterの間で、ブロックがあるならその位置、ないならafterの位置が帰ってくる
		const Vector3 hitPos = Mapchip::GetInstance()->HitMap(beforePos, afterPos, 1.0f);

		// もし違う場合(地面に触れる場合)
		if (hitPos.y != afterPos.y) {
			if (velocity_.y < 0.f) {
				// 地面に触れている
				isGround_ = true;
			}
			velocity_.y = 0.f;
		}
		
		// もし地面に触れているなら位置を当たった位置へ
		if (hitPos != afterPos) {
			worldTrans_->translation_ = hitPos;
		}
	}

	// もし地面に触れていたら
	if (isGround_) {
		//velocity_.x = 0.f;

		if (velocity_.y < 0.f) {
			velocity_.y = 0.f;
		}
	}

	// 加速度を初期化
	acceleration_ = {};
#pragma endregion

	// 行列を更新
	worldTrans_->UpdateMatrix();


	// デバッグ用
	ImGui::Begin("debug");
	ImGui::Text("%f , %f", worldTrans_->matWorld_.m[3][0], worldTrans_->matWorld_.m[3][1]);
	if (isGround_) {
		ImGui::Text("isGround : true");
	}
	else {
		ImGui::Text("isGround : false");
	}
	if (isJump) {
		ImGui::Text("isJump : true");
	}
	else {
		ImGui::Text("isJump : false");
	}
	ImGui::End();
}

void TmpObject::Draw(const ViewProjection& viewProjection) {
	if (model_ == nullptr) {
		return;
	}
	if (worldTrans_) {
		model_->Draw(*worldTrans_.get(), viewProjection);
	}
}
