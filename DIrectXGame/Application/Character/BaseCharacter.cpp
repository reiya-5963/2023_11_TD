#include "BaseCharacter.h"

void BaseCharacter::Initialize(const std::vector<Model*>& models) { 
	models_ = models;
	worldTrans_.Initialize();
	Collider::Initialize();

}

void BaseCharacter::Update() {
	UpdateWorldTransform();

	worldTrans_.UpdateMatrix();

}

void BaseCharacter::Draw(const ViewProjection& viewProjection) {
	for (Model* model : models_) {
		model->Draw(worldTrans_, viewProjection);
	}
}

Vector3 BaseCharacter::GetWorldPosition() {
	Vector3 result{};
	//
	result.x = worldTrans_.matWorld_.m[3][0];
	result.y = worldTrans_.matWorld_.m[3][1];
	result.z = worldTrans_.matWorld_.m[3][2];

	return result;
}
