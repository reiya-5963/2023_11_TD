#pragma once
#include "IGimmick.h"
#include "WallGimmick.h"
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
	enum BlockDirection {
		kLeft,
		kRight,
		kTop,
		kBottom,
	};

	struct ParentBlock {
		BlockDirection direction_; // 方向
		Vector3 endPoint_;
		int number_;	// 数
	};

	struct EasePointInfo {
		Vector3 startSquares = {};
		Vector3 endSquares = {};
		float valueY = 0.01f;
	};

private:
	EasePointInfo GenerateEaseStartToEnd(const Vector3& start,const Vector3& end, float value);

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

	void Reset();

	std::list<IGimmick*> GetGimmicks() {return gimmicks_;}
private:
	/// <summary>
	/// ギミックのリスト
	/// </summary>
	std::list<IGimmick*> gimmicks_;
	/// <summary>
	/// フロアモデル
	/// </summary>
	std::unique_ptr<Model> floarModel_;

	uint32_t blockTexture_ = 0u;

public:
	std::list<IGimmick*> GetGimmickList() { return gimmicks_; }

private:
	void AddWallGimmick(const Vector3& position, const Vector3& endPoint, ParentBlock info);
	void AddWallChild(const Vector3& position, WallGimmick* parent);
	void AddFloarGimmick();
	void AddDoorGimmick();
};

