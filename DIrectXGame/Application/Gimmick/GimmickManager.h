#pragma once
#include "IGimmick.h"
#include "viewProjection.h"
#include "Model.h"

#include <list>
#include <memory>

class GimmickManager
{
public:
	GimmickManager();
	~GimmickManager() = default;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection);

private:
	/// <summary>
	/// ギミックのリスト
	/// </summary>
	std::list<IGimmick*> gimmicks_;
	/// <summary>
	/// フロアモデル
	/// </summary>
	std::unique_ptr<Model> floarModel_;

public:
	std::list<IGimmick*> GetGimmickList() { return gimmicks_; }

private:
	void AddWallGimmick(const Vector3& position, WorldTransform* parent);
	void AddFloarGimmick();
	void AddDoorGimmick();
};

