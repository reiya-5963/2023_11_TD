#include "Collider.h"

void Collider::Initialize() {
	worldTransform_.Initialize();
}

void Collider::UpdateWorldTransform() {
	worldTransform_.translation_ = GetWorldPosition();
	worldTransform_.UpdateMatrix();
}

void Collider::Draw(Model* model, const ViewProjection& viewProjection) {
	model->Draw(worldTransform_, viewProjection);
}

void Collider::SetRadius(const Vector3 radius) {
	radius_ = radius;
	worldTransform_.scale_ = radius_;
	worldTransform_.matWorld_ = R_Math::MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
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
