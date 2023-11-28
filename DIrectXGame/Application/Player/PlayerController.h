#pragma once
#include <memory>
#include <optional>

#include "Player.h"
#include "Model.h"
#include "viewProjection.h"
#include "Gimmick/GimmickManager.h"

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
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="viewprojection"></param>
	void Draw(ViewProjection& viewprojection);
	/// <summary>
	/// 操作権の変更
	/// </summary>
	void ChangeControl();

	// コライダーとしてのプレイヤーを貸出
	Collider* GetPlayer1();
	Collider* GetPlayer2();


private:
	std::unique_ptr<Player> player1_;
	std::unique_ptr<Player> player2_;

public:
	/*Player* GetPlayer1() { return player1_.get(); }
	Player* GetPlayer2() { return player2_.get(); }*/

private:
	/// <summary>
	/// リスト用
	/// </summary>
	GimmickManager* gimmickManager_ = nullptr;

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

};

