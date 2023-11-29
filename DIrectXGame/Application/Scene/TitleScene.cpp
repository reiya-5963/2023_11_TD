#include "TitleScene.h"
#include "SceneManager.h"
TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	sceneManager_ = SceneManager::GetInstance();
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("GAME");
	}
}

void TitleScene::Draw() 
{

}
