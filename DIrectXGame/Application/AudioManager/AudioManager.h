#pragma once
#include "audio.h"
#include <vector>

class AudioManager
{
public:
	AudioManager();

	static AudioManager* GetInstance();

	/// <summary>
	/// BGM用の再生
	/// </summary>
	/// <param name="soundHandle"></param>
	void PlayBGMAudio(uint32_t soundHandle, float volume);
	/// <summary>
	/// SE用の再生
	/// </summary>
	/// <param name="soundHandle"></param>
	void PlaySEAudio(uint32_t soundHandle, float volume);
	/// <summary>
	/// 再生を停止
	/// </summary>
	/// <param name="soundHandle"></param>
	void StopAudio(uint32_t soundHandle);
	/// <summary>
	/// サウンド調整
	/// </summary>
	/// <param name="soundHandle"></param>
	/// <param name="volume"></param>
	//void SetVolume(uint32_t soundHandle, float volume);

	static uint32_t GetSoundList(int num) { return soundList_[num]; }

private:
	/// <summary>
	/// サウンドリスト
	/// </summary>
	static std::vector<uint32_t> soundList_;

	/// <summary>
	/// Audioインスタンス
	/// </summary>
	Audio* audio_ = nullptr;

public:
	enum SoundNumber {
		kAllBGM,
		kJump,
		kSwicth,
		kGhost,
		kSelectSE,
		kGoalSE,
	};

};

