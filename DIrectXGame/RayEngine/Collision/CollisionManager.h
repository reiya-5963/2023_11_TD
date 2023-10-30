#pragma once
#include "Collider.h"
#include <list>

#include "Model.h"

/// <summary>
/// 当たり判定管理クラス
/// </summary>
class CollisionManager {
public: // メンバ関数
	void Initialize();

	void UpdateWorldTransform();

	void Draw(const ViewProjection& viewProjection);
	/// <summary>
	/// 全ての当たり判定
	/// </summary>
	void CheckAllCollisions();

	/// <summary>
	/// 判定にオブジェクトを追加
	/// </summary>
	/// <param name="collider"></param>
	void AddColliders(Collider* collider);

	/// <summary>
	/// コンテナの初期化
	/// </summary>
	void ClearColliders();

private: // メンバ関数
	/// <summary>
	/// 二つのオブジェクトの当たり判定
	/// </summary>
	/// <param name="colliderA">オブジェクトA</param>
	/// <param name="colliderB">オブジェクトB</param>
	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

	bool IsCollision(Collider* colliderA, Collider* colliderB);

	//bool IsCollision(const Collider* colliderA, const Collider* colliderB);/*{
		/*Vector3 clossestPoint{
			std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
			std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
			std::clamp(sphere.center.z, aabb.min.z, aabb.max.z)
		};

		float distance = Length(Subtract(clossestPoint, sphere.center));
		if (distance <= sphere.radius) {
			return true;
		}

		return false;

	}*/
private: // メンバ変数
	// コライダーリスト
	std::list<Collider*> colliders_;

	std::unique_ptr<Model> model_;
};

