#include "AudioManager.h"
#include "audio.h"

std::vector<uint32_t> AudioManager::soundList_;

AudioManager::AudioManager()
{
	audio_ = Audio::GetInstance();
	uint32_t sound = audio_->LoadWave("sound/BGM.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/JumpSE.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/SwitchSE.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/GhostSE.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/SelectSE.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/GimmickSE.wav");
	soundList_.push_back(sound);
	sound = audio_->LoadWave("sound/ClearBGM.wav");
	soundList_.push_back(sound);
}

AudioManager* AudioManager::GetInstance()
{
	static AudioManager instance;
	return &instance;
}

void AudioManager::PlayBGMAudio(uint32_t soundHandle, float volume)
{
	if (!Audio::GetInstance()->IsPlaying(soundHandle)) {
		Audio::GetInstance()->PlayWave(soundHandle, true, volume);
		Audio::GetInstance()->SetVolume(soundHandle, volume);
	}
}

void AudioManager::PlaySEAudio(uint32_t soundHandle, float volume)
{
	Audio::GetInstance()->PlayWave(soundHandle, false, volume);
	Audio::GetInstance()->SetVolume(soundHandle, volume);
}

void AudioManager::StopAudio(uint32_t soundHandle)
{
	Audio::GetInstance()->StopWave(soundHandle);
}
