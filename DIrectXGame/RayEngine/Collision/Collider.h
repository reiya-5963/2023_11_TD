#pragma once
#include "MyStruct.h"
#include <cstdint>
#include "CollisionConfig.h"
#include "Model.h"

/// <summary>
/// 衝突判定オブジェクト
/// </summary>
class Collider {
private:
	// 衝突属性 (自分)
	uint32_t collisionAttribute_ = 0xffffffff;

	// 衝突マスク (相手)
	uint32_t CollisionMask_ = 0xffffffff;

public: // のちのち消すやつ
	Vector3 GetMin() { return min_; }

	Vector3 GetMax() { return max_; }

	void SetMin(const Vector3 min);

	void SetMax(const Vector3 max);

public: // メンバ関数
	// 初期化
	void Initialize();

	// ワールド変換データ更新
	void UpdateCollider();

	// 描画
	void Draw(Model* model, const ViewProjection& viewProjection);
	
	// 衝突時に呼ばれる関数
	virtual void OnCollision([[maybe_unused]]Collider* other) = 0;


public: // Getter & Setter
#pragma region Getter_Setter
	// 半径の取得
	Vector3 GetRadius() { return ColliderRadius_; }

	// 半径の設定
	void SetRadius(const Vector3 radius);

	// 衝突属性 (自分)
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(uint32_t collisionAttribute);

	// 衝突マスク (相手)
	uint32_t GetCollisionMask();
	void SetCollisionMask(uint32_t collisionMask);


	virtual Vector3 GetWorldPosition() = 0;
	
	/*WorldTransform* GetParent() { return parent_; }
	void SetParent(WorldTransform* parent) {
		parent_ = parent;
	}*/

	WorldTransform& GetCollisionWorldTransform() { return collisionWorldTransform_; }
	void SetCollisionWorldTransform(WorldTransform worldTrans) {

		collisionWorldTransform_ = worldTrans;
	}

	//種別ID
	uint32_t GetTypeID() const { return typeID_; }
	void SetTypeID(uint32_t typeID) { typeID_ = typeID; }

	virtual void SetParent(WorldTransform* parent) = 0;
	virtual WorldTransform* GetParent() = 0;

#pragma endregion

protected: // メンバ変数　
	// 衝突半径
	Vector3 ColliderRadius_ = {1.0f, 1.0f, 1.0f};
	
	// AABBの最少点
	Vector3 min_;
	
	// AABBの最大点
	Vector3 max_;

	WorldTransform collisionWorldTransform_;

	// 種別ID
	uint32_t typeID_ = 0u;
};

