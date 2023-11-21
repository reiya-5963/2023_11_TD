#pragma once
#include "Player/InputState.h"
#include "viewProjection.h"
#include "WorldTransform.h"
#include "Model.h"
#include "Vector3.h"
#include "Input.h"
#include "BaseCharacter.h"

#include <stdint.h>
#include <optional>

class InputState;

class Player : public BaseCharacter
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(const std::vector<Model*>& models) override;
	/// <summary>
	/// 設定
	/// </summary>
	/// <param name="position"></param>
	/// <param name="color"></param>
	void Setting(const Vector3& position, uint32_t color);
	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewProjection"></param>
	void Draw(const ViewProjection& viewProjection) override;

	void OnCollision(WorldTransform* worldTransform) override;
	
	void MoveUpdate(Vector3& moveDirect);

public: // アクセッサ
	/// <summary>
	/// 速さ設定
	/// </summary>
	/// <param name="velocity"></param>
	void SetVelocity(Vector3& velocity) { velocity_ = velocity; }
	/// <summary>
	/// 取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetVelocity() { return velocity_; }

	/// <summary>
	/// 入力設定
	/// </summary>
	/// <param name="state"></param>
	void SetState(InputState* state);

public:
	/// <summary>
	/// 状態
	/// </summary>
	enum class Behavior : int
	{
		kRoot,
		kJump,
		kAction,
	};

	/// <summary>
	/// アクション状態の取得
	/// </summary>
	/// <returns></returns>
	Behavior GetBehaviorState() { return behavior_; }

private:
	// 状態管理変数
	Behavior behavior_ = Behavior::kRoot;
	// 状態のリクエスト
	std::optional<Behavior> request_ = std::nullopt;

private:
	void RootInitialize();
	void RootUpdate();
	void JumpInitialize();
	void JumpUpdate();
	void ActionInitialize();
	void ActionUpdate();

public:
	/// <summary>
	/// 外部リクエスト
	/// </summary>
	/// <param name="request"></param>
	void SetBehaviorRequest(Behavior request) { request_ = request; }

private:
	// プレイヤーの入力状態
	InputState* inputState_ = nullptr;
	int test = 0;

public:
	InputState* GetInputState() { return inputState_; }
};

