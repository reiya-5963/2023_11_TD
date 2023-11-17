#pragma once

#include <memory>

#include "Model.h"
#include "WorldTransform.h"

class TmpObject {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection &viewProjection);
	
private:
	std::unique_ptr<Model> model_;
	std::unique_ptr<WorldTransform> worldTrans_;

	
	Vector3 velocity_{};
	Vector3 acceleration_{};
	//Vector3 beforePos = {};
	//Vector3 afterPos = {};

	
	//bool isCollider_ = false;

	bool isDefault = true;
	bool isJump = false;

	bool isGround_ = false;
	
	bool isMapChipCollision_ = true;

};

