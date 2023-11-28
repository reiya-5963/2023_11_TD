#include "CollisionManager.h"
#include <iterator>

#include "R_Math.h"
#include "GlobalVariables.h"

void CollisionManager::Initialize() {
	model_.reset(Model::CreateFlomObj("BaseElipse"));
	InitializeGlobalVariables();
}

void CollisionManager::UpdateWorldTransform() {
	ApplyGlobalVariables();

	for (Collider* collider : colliders_) {
		collider->UpdateCollider();
	}
}

void CollisionManager::Draw(const ViewProjection& viewProjection) {
	if (isColliderDraw) {
		for (Collider* collider : colliders_) {
			collider->Draw(model_.get(), viewProjection);
		}
	}
}

void CollisionManager::CheckAllCollisions() {
	// リスト内のペアを総当たり
	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); ++itrA) {
		// イテレータAからコライダーAを取得する
		Collider* colA = *itrA;

		// イテレータBはイテレータAの次の要素から回す (重複判定を回避)
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); ++itrB) {
			// イテレータBからコライダーBを取得する
 			Collider* colB = *itrB;

			// ペアの当たり判定
			CheckCollisionPair(colA, colB);
		}
	}
}

void CollisionManager::AddColliders(Collider* collider) {
	// コンテナに追加
	colliders_.push_back(collider);
}

void CollisionManager::ClearColliders() {
	// コンテナを初期化
	if (colliders_.size() != 0) {
		colliders_.clear();
	}
}

void CollisionManager::InitializeGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Colliders";
	//
	GlobalVariables::GetInstance()->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ColliderDraw", isColliderDraw);
}

void CollisionManager::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Colliders";
	isColliderDraw = globalVariables->GetIntValue(groupName, "ColliderDraw");
}

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {

	// 衝突フィルタリング (もし同じ属性なら判定しない)
	if (
		((colliderA->GetCollisionAttribute() & colliderB->GetCollisionMask()) == 0u) ||
		((colliderB->GetCollisionAttribute() & colliderA->GetCollisionMask()) == 0u)) {
		return;
	}
	//	colliderA->SetParent(nullptr);
		//colliderB->SetParent(nullptr);


		// もし当たってたら
	if (IsCollision(colliderA, colliderB)) {
		colliderA->OnCollision(colliderB);
		//colliderA->SetParent(&colliderB->GetCollisionWorldTransform());
		colliderB->OnCollision(colliderA);
		//	colliderB->SetParent(&colliderA->GetCollisionWorldTransform());
	}
}

bool CollisionManager::IsCollision(Collider* colliderA, Collider* colliderB)
{
	const float kError = 0.5f;
	if ((colliderA->GetMin().x - kError <= colliderB->GetMax().x + kError && colliderA->GetMax().x + kError >= colliderB->GetMin().x - kError) &&
		(colliderA->GetMin().y - kError <= colliderB->GetMax().y + kError && colliderA->GetMax().y + kError >= colliderB->GetMin().y - kError) &&
		(colliderA->GetMin().z - kError <= colliderB->GetMax().z + kError && colliderA->GetMax().z + kError >= colliderB->GetMin().z - kError)) {

		return true;
	}

	return false;
}
