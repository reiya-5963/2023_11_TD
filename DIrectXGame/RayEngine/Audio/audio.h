#pragma once

#include <cassert>
#include <array>
#include <cstdint>
#include <unordered_map>
#include <string>
#include <set>

#include <xaudio2.h>

#include <fstream>

#include <wrl.h>

class Audio {
public:
	// サウンドデータの最大数
	static const int kMaxSoundData = 256;

	// チャンクヘッダ
	struct ChunkHeader {
		char id[4]; //!< チャンク毎のID
		int32_t size; //!< チャンクサイズ
	};

	// RIFFチャンクヘッダ
	struct RiffHeader {
		ChunkHeader chunk; //!< "RIFF"
		char type[4]; //!< "WAVE"
	};

	// FMTチャンク
	struct FormatChunk {
		ChunkHeader chunk; //!< "FMT"
		WAVEFORMATEX fmt; //!< 波形フォーマット
	};

	// 音声データ
	struct SoundData {
		// 波形フォーマット
		WAVEFORMATEX wfex;
		// バッファの先頭サイズ
		BYTE* pBuffer;
		// バッファのサイズ
		unsigned int bufferSize;
		// 名前
		std::string name;
	};

	// 再生データ
	struct Voice {
		uint32_t handle = 0u;
		IXAudio2SourceVoice* sourceVoice = nullptr;
	};

	static Audio* GetInstance();

	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="directoryPath">音データが入っているパス</param>
	void Initilize(const std::string& directoryPath = "Resources/");

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	
	uint32_t LoadWave(const std::string& filename);

	void Unload(SoundData *soundData);

	uint32_t PlayWave(uint32_t soundDataHandle, bool loopFlag = false, float volume = 1.0f);

	void StopWave(uint32_t voiceHandle);

	bool IsPlaying(uint32_t voiceHandle);

	void SetVolume(uint32_t voiceHandle, float volume);

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	const Audio& operator=(const Audio&) = delete;


private:
	// XAudio2インスタンス
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	// サウンドデータコンテナ
	std::array<SoundData, kMaxSoundData> soundDatas_;
	// 再生中データコンテナ
	std::set<Voice*> voices_;
	// サウンド格納ディレクトリ
	std::string directoryPath_;
	// 次に使うサウンドデータの番号
	uint32_t indexSoundData_ = 0u;
	// 次に使う再生中データの番号
	uint32_t indexVoice_ = 0u;
	// オーディオコールバック
	//XAudio2VoiceCallback voiceCallback_;


};

