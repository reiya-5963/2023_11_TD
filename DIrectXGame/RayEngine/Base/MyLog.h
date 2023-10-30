#pragma once
#include <string>
#include <format>
using namespace std;

class MyLog {
public:
	// string->wstring
	static std::wstring ConvertString(const std::string& str);

	//wstring->string
	static 	std::string ConvertString(const std::wstring& str);

	//出力ウィンドウに文字出すやつ
	static void Log(const std::string& message);
};
