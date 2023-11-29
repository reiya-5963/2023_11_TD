#pragma once
#include "BaseScene.h"

#include <memory>
#include "FollowCamera.h"
#include "Mapchip.h"
//#include "TmpObject.h"
#include "Gimmick/GimmickManager.h"
#include "Player/PlayerController.h"
#include "Collision/CollisionManager.h"
#include "TransitionManager.h"
#include "FocusCamera.h"
#include "Goal.h"
#include "audio.h"
//#include "skydome.h"


/// <summary>
/// ゲームシーン
/// </summary>
class GameScene : public BaseScene {
public:
	 
	/// <summary>
	///	コンストラクタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	void Retry();


	void ColliderUpdate();
private:
	/*/// <summary>
	/// コライダーリストの更新
	/// </summary>
	void ColliderListUpdate();*/

	/// <summary>
	/// カメラ関係の更新
	/// </summary>
	void CameraUpdate();

private:
	Audio* audio_ = nullptr;
	TransitionManager* transitionmanager_ = nullptr;

private:
	Mapchip* map_;
	//std::unique_ptr<TmpObject> player_;

	std::unique_ptr<PlayerController> playerController_;
	std::unique_ptr<GimmickManager> gimmickManager_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	std::unique_ptr<FocusCamera> focusCamera_;

	std::unique_ptr<CollisionManager> colliderManager_ = nullptr;

	//std::unique_ptr<Model> skyModel;
	//std::unique_ptr<Skydome> back_ = nullptr;
	std::unique_ptr<Sprite> back_ = nullptr;
	uint32_t backTex_ = 0u;

	std::unique_ptr<Goal> goal_;
	std::unique_ptr<Model> goalModel_;


	std::unique_ptr<Sprite> retry_ = nullptr;
	Vector2 retryPos_ = { 0.0f, 0.0f };
	uint32_t retryTex_ = 0u;
};