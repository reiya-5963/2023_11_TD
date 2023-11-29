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

	struct SpriteValue 
	{
		Vector2 position_;
		Vector2 size_;
		float rotate_;
		Vector4 color_;
	};

	struct TransitionValue {
		bool isNow_;	// 行っているか
		float ease_t_;	// 補間レート
		int endFrame_;	// 終了フレーム
		float alphaValue_;	// 画像のアルファ値
		SpriteValue spriteInfo_;	// スプライトに渡す情報
	};

	TransitionValue blackOut_;

};

