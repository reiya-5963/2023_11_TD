#include "TitleScene.h"
#include "SceneManager.h"
TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	sceneManager_ = SceneManager::GetInstance();
	transition_ = TransitionManager::GetInstance();
}

void TitleScene::Finalize() {
}

void TitleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene("GAME");
	}
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		transition_->TransitionSetting();
	}
	if (transition_->GetIsBlackOutHalf()) {
		SceneManager::GetInstance()->ChangeScene("GAME");
	}

	transition_->Update();
}

void TitleScene::Draw() 
{
	transition_->Draw();
}
