#pragma once

#include "Input.h"
#include "DXCommon.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ParticleManager.h"
#include "viewProjection.h"
#include "WorldTransform.h"
#include "Triangle.h"

#include "CollisionManager.h"

class SceneManager;

class BaseScene {
public:

	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~BaseScene() = default;

	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	virtual void SetSceneManager(SceneManager* sceneManager) {
		sceneManager_ = sceneManager;
	}
protected:
	// dxCommon
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;

	// シーンマネージャー(借り物)
	SceneManager* sceneManager_ = nullptr;
};

