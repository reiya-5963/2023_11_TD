#include "BaseCharacter.h"
#include <cassert>
#include "ImGuiManager.h"
#include "Mapchip.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models) { 
	// モデルがあるかチェック
	for (uint32_t i = 0; i < models.size(); i++) {
		assert(models[i]);
	}
	
	// モデルを設定
	models_ = models;
	
	// オブジェクトのワールド変換データ初期化
	objectWorldTransform_.Initialize();
	
	// 衝突判定系を初期化
	Collider::Initialize();

	// それぞれ初期化
	velocity_ = {};
	acceleration_ = {};

}

void BaseCharacter::Update() {
	const float deltaTime = 1.0f / 60.0f;

#pragma region 共通の処理	

	objectWorldTransform_.UpdateMatrix();

	Vector3 worldPos = { objectWorldTransform_.matWorld_.m[3][0], objectWorldTransform_.matWorld_.m[3][1],objectWorldTransform_.matWorld_.m[3][2] };

	// 動く前の位置と、動いた後の位置を取得
	Vector3 beforePos = worldPos;
	Vector3& afterPos = worldPos;

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
	worldPos += fixVelocity;
	isGround_ = false;


	// もしマップチップ当たり判定をとるなら
	if (isMapChipCollision_ && !isGround_) {
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
			if (objectWorldTransform_.parent_) {
				worldPos.x = hitPos.x;
				worldPos.z = hitPos.z;
			}
			else {
				worldPos = hitPos;

			}
		}
	}

	if (objectWorldTransform_.parent_) {
		if(velocity_.y < 0.0f){
			isGround_ = true;
		}
		Vector3 parentWorldPos = { objectWorldTransform_.parent_->matWorld_.m[3][0], objectWorldTransform_.parent_->matWorld_.m[3][1], objectWorldTransform_.parent_->matWorld_.m[3][2] };
		objectWorldTransform_.translation_ = worldPos - parentWorldPos;
	}
	else {
		objectWorldTransform_.translation_ = worldPos;
	}

	// もし地面に触れていたら
	if (isGround_) {
		//velocity_.x = 0.f;

		if (velocity_.y < 0.f) {
			velocity_.y = 0.f;
		}
	}

	
#pragma endregion
	// オブジェクトのワールド変換データを更新
	objectWorldTransform_.UpdateMatrix();
	collisionWorldTransform_.translation_ = objectWorldTransform_.translation_;
	// 当たり判定用のワールド変換データを更新
	UpdateCollider();


#ifdef _DEBUG
	// デバッグ用
	ImGui::Begin("debug");
	ImGui::Text("%f , %f", objectWorldTransform_.matWorld_.m[3][0], objectWorldTransform_.matWorld_.m[3][1]);
	ImGui::Text("%f , %f", objectWorldTransform_.translation_.x, objectWorldTransform_.translation_.y);
	if (isGround_) {
		ImGui::Text("isGround : true");
	}
	else {
		ImGui::Text("isGround : false");
	}
	ImGui::End();
#endif // DEBUG
}

void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	for (Model* model : models_) {
		if (model) {
			model->Draw(objectWorldTransform_, viewProjection);
		}
	}
}

Vector3 BaseCharacter::GetWorldPosition() {
	Vector3 result{};
	//
	result.x = objectWorldTransform_.matWorld_.m[3][0];
	result.y = objectWorldTransform_.matWorld_.m[3][1];
	result.z = objectWorldTransform_.matWorld_.m[3][2];

	return result;
}
