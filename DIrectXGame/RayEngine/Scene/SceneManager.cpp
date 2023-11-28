#include "SceneManager.h"
#include <cassert>
#include "SceneFactory.h"


SceneManager* SceneManager::GetInstance() {
	static SceneManager instance;
	return &instance;
}

void SceneManager::Initialize() {	
	//BaseScene* scene = new GameScene();
	sceneFactory_ = new SceneFactory();
	scene_ = sceneFactory_->CreateScene("GAME");
	scene_->Initialize();
}

void SceneManager::Finalize() {
	scene_->Finalize();
	delete scene_;
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);

	//
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}

void SceneManager::SetSceneFactory(AbstractSceneFactory* sceneFactory) {
	sceneFactory_ = sceneFactory;
}

void SceneManager::Update() {
	// シーンのきりかえ　//
	// 次のシーンの予約があるなら
	if (nextScene_) {
		// 旧シーンの終了
		if (scene_) {
			scene_->Finalize();
			delete scene_;
		}

		// シーンの切り替え
		scene_ = nextScene_;
		nextScene_ = nullptr;
		scene_->SetSceneManager(this);
		// 次シーンを初期化する
		scene_->Initialize();
	}


	// 実行中のシーンを更新する
	scene_->Update();
}

void SceneManager::Draw() {
	scene_->Draw();
}
