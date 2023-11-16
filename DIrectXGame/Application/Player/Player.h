#pragma once
#include "PlayerState.h"
#include "viewProjection.h"
#include "WorldTransform.h"
#include "Model.h"
#include "Vector3.h"
#include "Input.h"

#include <stdint.h>
#include <optional>

class PlayerState;

class Player
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model);
	/// <summary>
	/// 設定
	/// </summary>
	/// <param name="position"></param>
	/// <param name="color"></param>
	void Setting(const Vector3& position, uint32_t color);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(ViewProjection& viewProjection);
	
	void MoveUpdate(Vector3& moveDirect);

	void SetState(PlayerState* state);

public:
	/// <summary>
	/// 状態
	/// </summary>
	enum class Behavior : uint32_t
	{
		kRoot,
		kJump,
		kAction,
	};

private:
	// 状態管理変数
	Behavior behavior_ = Behavior::kRoot;
	// 状態のリクエスト
	std::optional<Behavior> request_ = std::nullopt;

public:
	/// <summary>
	/// 外部リクエスト
	/// </summary>
	/// <param name="request"></param>
	void SetBehaviorRequest(Behavior request) { request_ = request; }

private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// モデル
	Model* model_ = nullptr;
	// プレイヤーの入力状態
	PlayerState* state_ = nullptr;

};

