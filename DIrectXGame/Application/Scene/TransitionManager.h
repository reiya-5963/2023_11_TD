#pragma once
#include "Model.h"
#include "Sprite.h"
#include "DXCommon.h"
#include "audio.h"
#include <memory>

class TransitionManager
{
private:
	DirectXCommon* dxCommon_ = nullptr;
	Audio* audio_ = nullptr;

public:
	TransitionManager();
	~TransitionManager() = default;

	/// <summary>
	/// シングルトン
	/// </summary>
	/// <returns></returns>
	static TransitionManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(/*Model* model*/) {};

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 遷移の呼び出し
	/// </summary>
	void TransitionSetting();

	/// <summary>
	/// ブラックアウト処理
	/// </summary>
	void BlackOutProcess();

private:


	std::unique_ptr<Sprite> whiteSprite_;

	/// <summary>
	/// スプライトの情報を入れる構造体
	/// </summary>
	struct SpriteValue 
	{
		Vector2 position_;	// 座標
		Vector2 size_;	// サイズ
		float rotate_;	// 角度
		Vector4 color_;	// 色
	};

	/// <summary>
	/// 遷移に使う構造体
	/// </summary>
	struct TransitionValue {
		bool isNow_;	// 行っているか
		bool isHalf_;	// 中間
		bool isChange_; // シーン変更タイミング
		float ease_t_;	// 補間レート
		int endFrame_;	// 終了フレーム
		SpriteValue spriteInfo_;	// スプライトに渡す情報
	};

	TransitionValue blackOut_;

public:
	bool GetIsBlackOutHalf() { return blackOut_.isChange_; }

};

