#include "MyGame.h"

// ウィンドウズアプリのエントリーポイント
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	// ゲームクラスの生成
	Framework* myGame = new MyGame;

	// 実行
	myGame->Run();

	// 開放
	delete myGame;

	return 0;
}

