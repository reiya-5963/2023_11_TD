#pragma once
#include "Gimmick/IGimmick.h"
#include "viewProjection.h"

class WallGimmick : public IGimmick
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model)override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection) override;
	/// <summary>
	/// 衝突
	/// </summary>
	void OnCollision([[maybe_unused]] Collider* other) override;
	void SetParent(WorldTransform* parent) { worldTransform_.parent_ = parent; };

	WorldTransform* GetParent() { return &worldTransform_; };

private:
	bool isAction_ = false;

};

