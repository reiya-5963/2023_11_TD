#pragma once

#include <vector>
#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "Collider.h"

/// <summary>
/// ベースキャラクター
/// </summary>
class BaseCharacter : public Collider {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models">モデルデータ配列</param>
	virtual void Initialize(const std::vector<Model*>& models);
	
	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection">ビュープロジェクション (参照渡し) </param>
	virtual void Draw(const ViewProjection& viewProjection);

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns>ワールド変換データ</returns>
	const WorldTransform& GetWorldTransform() { 
		return worldTransform_;
	}

	virtual Vector3 GetWorldPosition() override;

protected:
	// モデルデータ配列
	std::vector<Model*> models_;
	// ワールド変換データ
	//WorldTransform worldTrans_;
};