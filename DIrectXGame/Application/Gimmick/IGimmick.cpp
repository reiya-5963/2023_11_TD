#include "IGimmick.h"

void IGimmick::StartSetting(float maxFrame)
{
	if (!isAction_) {
		isAction_ = true;
		ease_T_ = 0;

		isBeforeStartup = true;

		// フレーム数
		maxFrame_ = maxFrame;
	}
}

void IGimmick::ReturnSetting(float maxFrame)
{
	if (!isAction_) {
		isAction_ = true;
		ease_T_ = 0;

		isBeforeStartup = false;

		// フレーム数
		maxFrame_ = maxFrame;
	}


}

void IGimmick::SetEasePoint(const Vector3& begin, const Vector3& end)
{
	beginPoint_ = begin;
	EndPoint_ = end;
}
