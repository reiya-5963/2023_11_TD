#include "IGimmick.h"

void IGimmick::StartSetting()
{
	if (!isAction_) {
		isAction_ = true;
		ease_T_ = 0;

		if (isBeforeStartup) {
			isBeforeStartup = false;
		}
		else {
			isBeforeStartup = true;
		}

	}
}

void IGimmick::SetEasePoint(const Vector3& begin, const Vector3& end)
{
	beginPoint_ = begin;
	EndPoint_ = end;
}
