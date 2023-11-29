#include "Goal.h"
#include "CollisionTypeIdDef.h"

void Goal::Initialize(Model* model) {
	model_ = model;

	this->collisionWorldTransform_.Initialize();
	ColliderRadius_ = { 2.0f,2.0f,2.0f };

	collisionWorldTransform_.translation_ = { 55.0f, 46.0f, 4.0f };
	collisionWorldTransform_.rotation_.y = 1.57f;
	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kGoal));
}

void Goal::Update() {
	collisionWorldTransform_.translation_ = { 55.0f, 46.0f, 4.0f };

	Collider::UpdateCollider();
	//collisionWorldTransform_.UpdateMatrix();
}

void Goal::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(collisionWorldTransform_, viewProjection);
}

void Goal::OnCollision([[maybe_unused]] Collider* other)
{
}
