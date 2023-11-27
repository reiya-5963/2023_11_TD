#pragma once
#include "Model.h"
#include <cassert>
#include "WorldTransform.h"

class Skydome {
public:
	void Initialize(Model* model, const Vector3& position);

	void Update();

	void Draw(ViewProjection& viewProjection);

private:
	WorldTransform worldTransform_;

	Model* model_ = nullptr;

};