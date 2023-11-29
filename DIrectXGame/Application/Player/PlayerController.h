#pragma once
#include <memory>
#include <optional>

#include "Player.h"
#include "Model.h"
#include "viewProjection.h"
#include "Gimmick/GimmickManager.h"
#include "Sprite.h"

struct Vector2_AABB{
	Vector2 min;
	Vector2 max;
};

class PlayerController
{
public:
	PlayerController();
	~PlayerController();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update(const ViewProjection& viewProjection);
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewprojection"></param>
	void Draw(ViewProjection& viewprojection);
	/// <summary>
	/// UI描画
	/// </summary>
	void UIDraw();

	void Reset();

	void PlayerPtrReset() {};

	void PtrSetting();

	// コライダーとしてのプレイヤーを貸出
	Collider* GetPlayer1();
	Collider* GetPlayer2();

private:
	/// <summary>
	/// スクリーン座標に変換
	/// </summary>
	/// <param name="position"></param>
	/// <param name="viewProjection"></param>
	/// <returns></returns>
	Vector3 WorldToScreen(const Vector3& position, const ViewProjection& viewProjection);
	/// <summary>
	/// 操作権の変更
	/// </summary>
	void ChangeControl();

	/// <summary>
	/// アクティブプレイヤーの近くのギミック探し
	/// </summary>
	/// <param name="viewProjection"></param>
	void ActivePlayerArea(const ViewProjection& viewProjection);

	/// <summary>
	/// インアクティブプレイヤーUI
	/// </summary>
	/// <param name="viewProjection"></param>
	void InactivePlayerInfo(const ViewProjection& viewProjection);

	/// <summary>
	/// 内側か判断
	/// </summary>
	/// <param name="player"></param>
	/// <returns></returns>
	bool PlayerInGimmick(Player* player);

private: // UI
	/// <summary>
	/// Bボタン
	/// </summary>
	std::unique_ptr<Sprite> BButtonUi_;
	/// <summary>
	/// Xボタン
	/// </summary>
	std::unique_ptr<Sprite> XButtonUi_;
	/// <summary>
	/// ゴールへの矢印
	/// </summary>
	std::unique_ptr<Sprite> ArrowUi_;
	/// <summary>
	/// ゴールしていないプレイヤー数
	/// </summary>
	std::unique_ptr<Sprite> remainPlayer_;



private:
	Vector2_AABB gim_;

	bool isInArea_ = false;

	Player* inactivePlayer_ = nullptr;
	Player* activePlayer_ = nullptr;

	Vector2 GenerateScreenPosition(const Vector3& worldPosition, const Vector3& offset, const ViewProjection& viewProjection);

	Vector3 goalPosition_ = {};
	Vector2 arrowScreenPosition_ = {};

	Audio* audio_ = nullptr;

private:
	std::unique_ptr<Player> player1_;
	std::unique_ptr<Player> player2_;

	bool isClear_ = false;

public:
	Player* GetPlayerPtr1() { return player1_.get(); }
	Player* GetPlayerPtr2() { return player2_.get(); }

	/// <summary>
	/// クリアフラグ
	/// </summary>
	bool GetIsClear() { return isClear_; }

	void SetGoalPosition(const Vector3& goalPosition) { goalPosition_ = goalPosition; }

private:
	/// <summary>
	/// リスト用
	/// </summary>
	GimmickManager* gimmickManager_ = nullptr;

	/// <summary>
	/// AABBの当たり判定
	/// </summary>
	bool IsAABBCollision(const Vector2_AABB& aabb1, const Vector2_AABB& aabb2);

public:
	/// <summary>
	/// 設定
	/// </summary>
	/// <param name="manager"></param>
	void SetGimmickManager(GimmickManager* manager) { gimmickManager_ = manager; }

private: // 切り替え用変数
	/// <summary>
	/// 切り替え
	/// </summary>
	enum class ControlNum : uint32_t 
	{
		kOne,
		kTwo,
	};

	ControlNum controlNum_;

	std::optional<ControlNum> changeRequest_ = std::nullopt;

	bool isDelay_ = 0;

	int delayCount_ = 0;

	IGimmick* tmpGimmick_;

	struct GhostWork {
		bool isDelay_;
		int delayCount_;
	};

	GhostWork ghostWork_;

private:
	uint32_t jumpSE_ = 0u;
	uint32_t switchSE_ = 0u;
	uint32_t ghostSE_ = 0u;

};

