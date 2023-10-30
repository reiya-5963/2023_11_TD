#include "MyGame.h"

void MyGame::Initialize() {
	// 基盤部分の初期化処理
	Framework::Initialize();
}

void MyGame::Finalize() {
	// 基盤部分の終了処理
	Framework::Finalize();
}

void MyGame::Update() {
	// 基盤部分の毎フレーム更新前処理
	Framework::PreUpdate();

	// もしESCAPEキーを押したらゲーム終了
	if (input_->TriggerKey(DIK_ESCAPE)) {
		endRequest_ = true;
	}

	// 基盤部分の毎フレーム更新後処理
	Framework::PostUpdate();
}

void MyGame::Draw() {
	// 描画前処理
	dxCommon_->preDraw();

	// シーンの描画
	sceneManager_->Draw();

	// ImGui描画処理
	imGuiManager_->Draw();

	// 描画後処理
	dxCommon_->postDraw();
}
