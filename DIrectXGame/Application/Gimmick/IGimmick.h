#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "viewProjection.h"
#include "Collider.h"

/// <summary>
/// ギミックのベースクラス
/// </summary>
class IGimmick : public Collider
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	virtual void Initialize(Model* model) = 0;
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection"></param>
	virtual void Draw(const ViewProjection& viewProjection) = 0;

public:
	/// <summary>
	/// 座標設定
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position) { worldTransform_.translation_ = position; }
	/// <summary>
	/// ワールド座標系
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition() { 
		return Vector3{ worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2] };
	}
	/// <summary>
	/// 移動前後の座標設定
	/// </summary>
	/// <param name="begin"></param>
	/// <param name="end"></param>
	void SetEasePoint(const Vector3& begin, const Vector3& end);

	void SetIsAction(bool actionFlag) { isAction_ = actionFlag; }

	void SetParentChildren(WorldTransform* parent) { worldTransform_.parent_ = parent; }

	WorldTransform* GetWorldTransform() { return &worldTransform_; }

protected:
	/// <summary>
	/// ワールドトランスフォーム
	/// </summary>
	WorldTransform worldTransform_;
	/// <summary>
	/// モデル
	/// </summary>
	Model* model_ = nullptr;
	/// <summary>
	/// 速さ
	/// </summary>
	Vector3 velocity_ = {};

	/// <summary>
	/// 始点
	/// </summary>
	Vector3 beginPoint_ = {};
	/// <summary>
	/// 終点
	/// </summary>
	Vector3 EndPoint_ = {};

	/// <summary>
	/// アクション中か
	/// </summary>
	bool isAction_ = false;
	/// <summary>
	/// 補間レート
	/// </summary>
	float ease_T_ = 0;

};

