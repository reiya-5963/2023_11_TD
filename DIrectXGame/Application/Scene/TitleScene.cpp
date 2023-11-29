#include "TitleScene.h"
#include "SceneManager.h"
#include "audio.h"

TitleScene::TitleScene() {
}

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {
	sceneManager_ = SceneManager::GetInstance();
	transition_ = TransitionManager::GetInstance();
	audioManager_ = new AudioManager();
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

	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		audioManager_->PlayBGMAudio(AudioManager::GetSoundList(AudioManager::kAllBGM), 0.1f);
	}

	transition_->Update();
}

void TitleScene::Draw() 
{
	transition_->Draw();
}
