#pragma once

#include <memory>

#include "Model.h"
#include "WorldTransform.h"
#include "BaseCharacter.h"

class TmpObject : public BaseCharacter {
public:
	void Initialize(const std::vector<Model*>& models) override;
	void Update() override;
	void Draw(const ViewProjection &viewProjection) override;
	void OnCollision() override {};

private:

	bool isDefault = true;
	bool isJump = false;
};

