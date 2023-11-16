#include "WallGimmick.h"

void WallGimmick::Draw(ViewProjection& viewProjection)
{

	model_->Draw(worldTransform_, viewProjection);

}
