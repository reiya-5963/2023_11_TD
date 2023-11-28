#pragma once
#include "Player/InputState.h"
#include "viewProjection.h"
#include "WorldTransform.h"
#include "Model.h"
#include "Vector3.h"
#include "Input.h"
#include "BaseCharacter.h"
#include "Gimmick/IGimmick.h"

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

	void OnCollision([[maybe_unused]] Collider* other)  override;
		
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

private: // 状態ごとの関数
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

	struct modelInfo {
		bool isLeft_;
		float defaultRotate_;
	};

	modelInfo info_;

	// 帽子用
	WorldTransform worldTransformHat_;
	std::unique_ptr<Model> hatModel_;

private: // 憑りつきアクション用
	// アクション定数
	struct ConstAction {
		uint32_t startTime_; // 憑りつくまで
		uint32_t endTime_;	// 終了まで
		uint32_t releaseTime_;	// 憑りつき解除まで 
	};

	static const ConstAction kConstAction_;

	// アクションステート
	enum class ActionState : uint32_t 
	{
		kReserve,
		kNow,
		kRelease,
	};

	ActionState actionState_;

	// アクション用の変数
	struct ActionValue {
		Vector3 startPoint_;
		Vector3 endPoint_;
		uint32_t timer_;
		float ease_t_;
	};

	ActionValue tmpValue_;

	IGimmick* iGimmickPtr_;

public:
	void GhostSetting();

	void SetGimmickPtr(IGimmick* gimmick) {
		iGimmickPtr_ = gimmick;
		iGimmickPtr_->SetIsSetup(true);
	}
public: // アクセッサ
#pragma region アクセッサ
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

		/// <summary>
		/// 親子設定
		/// </summary>
		/// <param name="parent"></param>
		void SetParent(WorldTransform* parent) { objectWorldTransform_.parent_ = parent; };

		/// <summary>
		/// 親子取得
		/// </summary>
		/// <returns></returns>
		WorldTransform* GetParent() { return &objectWorldTransform_; };
		
		/// <summary>
		/// 入力状態
		/// </summary>
		/// <returns></returns>
		InputState* GetInputState() { return inputState_; }

		/// <summary>
		/// 向き
		/// </summary>
		/// <param name="isLeft"></param>
		void SetIsLeft(bool isLeft) { info_.isLeft_ = isLeft; }

		bool GetIsGround() { return isGround_; }

#pragma endregion

};

