#pragma once
#include "Gimmick/IGimmick.h"
#include "viewProjection.h"

class WallGimmick : public IGimmick
{
public:
	void Update() override;
	void Draw(ViewProjection& viewProjection) override;

};

