#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "viewProjection.h"

/// <summary>
/// ギミックのベースクラス
/// </summary>
class IGimmick
{
public:

	virtual void Update() {};
	virtual void Draw(ViewProjection& viewProjection) = 0;

protected:
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

};

