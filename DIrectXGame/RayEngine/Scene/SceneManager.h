#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
class SceneManager {
public:

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static SceneManager* GetInstance();

	void Initialize();

	/// <summary>
	/// 
	/// </summary>
	void Finalize();

	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="nextScene"></param>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory);

	/// <summary>
	/// 
	/// </summary>
	void Update();

	/// <summary>
	/// 
	/// </summary>
	void Draw();

private:
	SceneManager() = default;
	~SceneManager() = default;
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator = (const SceneManager&) = delete;

private:
	AbstractSceneFactory* sceneFactory_ = nullptr;

	// 今のシーン
	BaseScene* scene_ = nullptr;
	
	// 次のシーン
	BaseScene* nextScene_ = nullptr;
};

