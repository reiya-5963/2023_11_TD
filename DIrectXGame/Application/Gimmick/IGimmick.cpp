#include "IGimmick.h"
#include "AudioManager/AudioManager.h"

void IGimmick::StartSetting(float maxFrame)
{
	if (!isAction_) {
		isAction_ = true;
		ease_T_ = 0;

		isBeforeStartup = true;

		AudioManager::GetInstance()->PlaySEAudio(AudioManager::GetSoundList(AudioManager::kGimmickSE), 0.1f);

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

		AudioManager::GetInstance()->PlaySEAudio(AudioManager::GetSoundList(AudioManager::kGimmickSE), 0.1f);

		// フレーム数
		maxFrame_ = maxFrame;
	}


}

void IGimmick::SetEasePoint(const Vector3& begin, const Vector3& end)
{
	beginPoint_ = begin;
	EndPoint_ = end;
}
