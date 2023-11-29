#include "audio.h"
#pragma comment(lib, "xaudio2.lib")

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

void Audio::Initilize(const std::string& directoryPath) {
	directoryPath_ = directoryPath;
	
	HRESULT result;
	
	IXAudio2MasteringVoice* masterVoice;

	// XAudio2エンジンのインスタンスを生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	// マスターボイスを生成(xAuidoの開放で自動deleteされるので注意)
	result = xAudio2_->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	indexSoundData_ = 0u;
	indexVoice_ = 0u;
}

void Audio::Finalize() {
	//
	xAudio2_.Reset();
	//
	for (auto& soundData : soundDatas_) {
		Unload(&soundData);
	}

}

uint32_t Audio::LoadWave(const std::string& filename) {
	assert(indexSoundData_ < kMaxSoundData);
	uint32_t handle = indexSoundData_;
	// 読み込み済みサウンドデータを検索
	auto it = std::find_if(soundDatas_.begin(), soundDatas_.end(), [&](const auto& soundData) {
		return soundData.name == filename;
	});
	if (it != soundDatas_.end()) {
		// 読み込み済みサウンドデータの要素番号を取得
		handle = static_cast<uint32_t>(std::distance(soundDatas_.begin(), it));
		return handle;
	}

	//
	bool currentRelative = false;
	if (2 < filename.size()) {
		currentRelative = (filename[0] == '.') && (filename[1] == '/');
	}
	std::string fullpath = currentRelative ? filename : directoryPath_ + filename;



	//HRESULT result;

#pragma region 1 ファイルオープン
	// ファイル入力ストリームのインスタンス
	std::ifstream file;
	// .wavファイルをバイナリモードで開く
	file.open(fullpath, std::ios_base::binary);
	// ファイルオープン失敗を検出する
	assert(file.is_open());
#pragma endregion
#pragma region 2 .wavデータ読み込み
	// RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	// ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	// タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	// Formatチャンクの読み込み
	FormatChunk format = {};
	// チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	// チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	// Dataチャンク読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//// JUNKチャンクを検出した場合
	//if (strncmp(data.id, "JUNK", 4) == 0) {
	//	// 読み取り位置をJUNKチャンクの終わりまで進める
	//	file.seekg(data.size, std::ios_base::cur);
	//	// 最読み込み
	//	file.read((char*)&data, sizeof(data));
	//}
	// JUNKチャンクを検出した場合
	while (_strnicmp(data.id, "junk", 4) == 0 || _strnicmp(data.id, "PAD ", 4) == 0 ||
		_strnicmp(data.id, "Fake", 4) == 0 || _strnicmp(data.id, "FLLR", 4) == 0 ||
		_strnicmp(data.id,"bext",4) == 0 ) {
		// 読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		// 再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (_strnicmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// data部かチェック
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	// Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

#pragma endregion
#pragma region 3 ファイルクローズ
	// Waveファイルを閉じる
	file.close();

#pragma endregion
#pragma region 4 読み込んだ音声データをreturn
	// returnするための
	SoundData& soundData = soundDatas_.at(handle);

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;
	soundData.name = filename;

	indexSoundData_++;

	return handle;
#pragma endregion
}

void Audio::Unload(SoundData* soundData) {
	// バッファのメモリを開放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

uint32_t Audio::PlayWave(uint32_t soundDataHandle, bool loopFlag, float volume) {
	HRESULT result;
	assert(soundDataHandle <= soundDatas_.size());

	// サウンドデータの参照を取得
	SoundData& soundData = soundDatas_.at(soundDataHandle);
	// 未読み込みの検出
	assert(soundData.bufferSize != 0);

	uint32_t handle = indexVoice_;

#pragma region 1 ソースボイスを生成
	// 波形フォーマットをもとにSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));
#pragma endregion

	// 再生中データ
	Voice* voice = new Voice();
	voice->handle = handle;
	voice->sourceVoice = pSourceVoice;
	// 再生中データコンテナに登録
	voices_.insert(voice);


#pragma region 2 波形データのセット
	// 再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;
	if (loopFlag) {
		// 無限ループ
		buf.LoopCount = XAUDIO2_LOOP_INFINITE;
	}

#pragma endregion

#pragma region 3 再生スタート
	// 波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();
#pragma endregion
	return handle;
}

void Audio::StopWave(uint32_t voiceHandle) {
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) {return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->DestroyVoice();

		voices_.erase(it);
	}
}

bool Audio::IsPlaying(uint32_t voiceHandle) {

	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) {return voice->handle == voiceHandle; });
	// 発見の判断
	if (it != voices_.end()) {
		XAUDIO2_VOICE_STATE state{};
		(*it)->sourceVoice->GetState(&state);
		return state.SamplesPlayed != 0;
	}

	return false;
}

void Audio::SetVolume(uint32_t voiceHandle, float volume)
{
	// 再生中リストから検索
	auto it = std::find_if(
		voices_.begin(), voices_.end(), [&](Voice* voice) { return voice->handle == voiceHandle; });
	// 発見
	if (it != voices_.end()) {
		(*it)->sourceVoice->SetVolume(volume);
	}
}
