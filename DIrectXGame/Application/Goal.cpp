#include "Goal.h"
#include "CollisionTypeIdDef.h"

void Goal::Initialize(Model* model)
{
	model_ = model;

	this->collisionWorldTransform_.Initialize();
	ColliderRadius_ = { 1.0f,1.0f,1.0f };

	SetTypeID(static_cast<uint32_t>(CollisionTypeIdDef::kGoal));
}

void Goal::Update()
{
	collisionWorldTransform_.UpdateMatrix();
}

void Goal::Draw(const ViewProjection& viewProjection)
{
	model_->Draw(collisionWorldTransform_, viewProjection);
}

void Goal::OnCollision([[maybe_unused]] Collider* other)
{
}
