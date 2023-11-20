#pragma once
#include <memory>
#include <optional>

#include "Player.h"
#include "Model.h"
#include "viewProjection.h"


class PlayerController
{
public:
	PlayerController();
	~PlayerController() = default;

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

private:
	std::unique_ptr<Player> player1_;
	std::unique_ptr<Player> player2_;

public:
	Player* GetPlayer1() { return player1_.get(); }
	Player* GetPlayer2() { return player2_.get(); }

private:
	enum class ControlNum : uint32_t 
	{
		kOne,
		kTwo,
	};

	ControlNum controlNum_;

	std::optional<ControlNum> changeRequest_ = std::nullopt;

	bool isDelay_ = 0;

	int delayCount_ = 0;
};

