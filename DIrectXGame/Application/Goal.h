#pragma once
#include "Collider.h"
#include "Model.h"

class Goal : public Collider
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model);
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection);

	void SetPosition(const Vector3& position) { collisionWorldTransform_.translation_ = position; }

	void OnCollision([[maybe_unused]] Collider* other) override;

	Vector3 GetWorldPosition() override {
		return Vector3{ collisionWorldTransform_.matWorld_.m[3][0],collisionWorldTransform_.matWorld_.m[3][1],collisionWorldTransform_.matWorld_.m[3][2] };
	}

	void SetParent(WorldTransform* parent) override { collisionWorldTransform_.parent_ = parent; }
	WorldTransform* GetParent() { return &collisionWorldTransform_; }

private:
	Model* model_ = nullptr;

};

