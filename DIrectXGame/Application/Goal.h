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

private:
	Model* model_ = nullptr;

};

