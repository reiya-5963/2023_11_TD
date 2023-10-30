#pragma once
#include "DXCommon.h"
#include <dxcapi.h>
#include <memory>
#include <string>

class DXCManager {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static DXCManager* GetInstance();


	/// <summary>
	///DXCの生成
	/// </summary>
	static void CreateDXC();

	/// <summary>
	/// 
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	static IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);


private:
	//DXC関連
	static  Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_;
	static	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_;
	static	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_;

};