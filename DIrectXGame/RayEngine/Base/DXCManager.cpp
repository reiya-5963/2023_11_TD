#include "DXCManager.h"
#include <cassert>
#include "MyLog.h"

#pragma comment(lib, "dxcompiler.lib")

Microsoft::WRL::ComPtr<IDxcUtils> DXCManager::dxcUtils_ = nullptr;
Microsoft::WRL::ComPtr<IDxcCompiler3> DXCManager::dxcCompiler_ = nullptr;
Microsoft::WRL::ComPtr<IDxcIncludeHandler> DXCManager::includeHandler_ = nullptr;


/// <summary>
/// 
/// </summary>
/// <returns></returns>
DXCManager* DXCManager::GetInstance() {
	static DXCManager instance;
	return &instance;
}

/// <summary>
///DXCの生成
/// </summary>
void DXCManager::CreateDXC() {
	HRESULT result = S_FALSE;

	result = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(result));
	result = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(result));

	result = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(result));
}

/// <summary>
/// 
/// </summary>
/// <param name="filePath"></param>
/// <param name="profile"></param>
/// <returns></returns>
IDxcBlob* DXCManager::CompileShader(
	const std::wstring& filePath,
	const wchar_t* profile) {
#pragma region 1.hlslファイルを読む
	//これからシェーダーをコンパイルする旨をログに出す
	MyLog::Log(MyLog::ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT result = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(result));
	//読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; //UTF8の文字コードであることを通知
#pragma endregion
#pragma region 2.Compileする
	LPCWSTR arguments[] = {
		filePath.c_str(),
		L"-E", L"main",
		L"-T", profile,
		L"-Zi", L"-Qembed_debug",
		L"-Od",
		L"-Zpr",
	};

	IDxcResult* shaderResult = nullptr;
	result = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_.Get(),
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(result));
#pragma endregion
#pragma region 3.警告.エラーが出ていないか確認する
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		MyLog::Log(shaderError->GetStringPointer());
		assert(false);
	}
#pragma endregion
#pragma region 4.Compile結果を受け取って返す
	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	result = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(result));
	//成功したらログをだす
	MyLog::Log(MyLog::ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	//もう使わないので開放
	shaderSource->Release();
	shaderResult->Release();
	//実行用のバイナリを返却
	return shaderBlob;
#pragma endregion
}

