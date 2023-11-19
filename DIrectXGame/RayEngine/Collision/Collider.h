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

public: // メンバ関数
	// 初期化
	void Initialize();

	// ワールド変換データ更新
	void UpdateWorldTransform();

	// 描画
	void Draw(Model* model, const ViewProjection& viewProjection);
	
	// 衝突時に呼ばれる関数
	virtual void OnCollision(WorldTransform* worldTransform) = 0;
	//virtual void OnCollisionExit() = 0;


public: // Getter & Setter
#pragma region Getter_Setter
	// 半径の取得
	Vector3 GetRadius() { return ColliderRadius_; }

	// 半径の設定
	void SetRadius(const Vector3 radius);

	Vector3 GetMin() { return min_; }

	Vector3 GetMax() { return max_; }

	void SetMin(const Vector3 min);

	void SetMax(const Vector3 max);

	// 衝突属性 (自分)
	uint32_t GetCollisionAttribute();
	void SetCollisionAttribute(uint32_t collisionAttribute);

	// 衝突マスク (相手)
	uint32_t GetCollisionMask();
	void SetCollisionMask(uint32_t collisionMask);


	virtual Vector3 GetWorldPosition() = 0;
	
	WorldTransform* GetParent() { return parent_; }
	void SetParent(WorldTransform* parent) {
		parent_ = parent;
	}

	WorldTransform& GetCollisionWorldTransform() { return collisionWorldTransform_; }
	void SetCollisionWorldTransform(WorldTransform worldTrans) {

		collisionWorldTransform_ = worldTrans;
	}

	bool GetIsCollision() { return isCollision_; }
	void SetIsCollision(bool isColision) { isCollision_ = isColision; }
	
	bool GetPreIsCollision() { return preIsCollision_; }
	void SetPreIsCollision(bool isColision) { preIsCollision_ = isColision; }
#pragma endregion

protected: // メンバ変数　
	// 衝突半径
	Vector3 ColliderRadius_ = {1.0f, 1.0f, 1.0f};
	// AABBの最少点
	Vector3 min_;
	// AABBの最大点
	Vector3 max_;

	WorldTransform collisionWorldTransform_;

	// 地面
	WorldTransform* parent_;

	bool isCollision_ = false;
	bool preIsCollision_ = false;;
};

