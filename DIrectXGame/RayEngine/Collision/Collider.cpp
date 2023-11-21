#include "Collider.h"

void Collider::Initialize() {
	collisionWorldTransform_.Initialize();
}

void Collider::UpdateCollider() {
	// AABB用のMinとMaxを設定
	SetMin(GetWorldPosition() - GetRadius());
	SetMax(GetWorldPosition() + GetRadius());
	collisionWorldTransform_.scale_ = GetRadius();
	// ワールド変換データを更新
	collisionWorldTransform_.UpdateMatrix();
}

void Collider::Draw(Model* model, const ViewProjection& viewProjection) {
	model->Draw(collisionWorldTransform_, viewProjection);
}

void Collider::SetRadius(const Vector3 radius) {
	ColliderRadius_ = radius;
	collisionWorldTransform_.scale_ = ColliderRadius_;
	collisionWorldTransform_.matWorld_ = R_Math::MakeAffineMatrix(collisionWorldTransform_.scale_, collisionWorldTransform_.rotation_, collisionWorldTransform_.translation_);
}

void Collider::SetMin(const Vector3 min){
	min_ = min;
}

void Collider::SetMax(const Vector3 max) {
	max_ = max;
}

uint32_t Collider::GetCollisionAttribute() {
	return collisionAttribute_;
}

void Collider::SetCollisionAttribute(uint32_t collisionAttribute) {
	collisionAttribute_ = collisionAttribute;
}

uint32_t Collider::GetCollisionMask() {
	return CollisionMask_;
}

void Collider::SetCollisionMask(uint32_t collisionMask) {
	CollisionMask_ = collisionMask;
}
