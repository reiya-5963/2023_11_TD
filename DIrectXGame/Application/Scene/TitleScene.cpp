#include "TitleScene.h"
#include "SceneManager.h"
TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");
	}
}

void TitleScene::Draw() {
}
