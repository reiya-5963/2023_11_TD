#pragma once
#include "TransitionManager.h"
#include "AudioManager/AudioManager.h"

#include "BaseScene.h"

class TitleScene : public BaseScene {
public:

	/// <summary>
	///	コンストラクタ
	/// </summary>
	TitleScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene();

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

	/// <summary>
	/// 幽霊の動き
	/// </summary>
	void GhostSpriteAnimation();

private:

	TransitionManager* transition_ = nullptr;
	AudioManager* audioManager_ = nullptr;

	float waveAnimation_t_ = 0;
	float amplitude_ = 5.0f;
	float fre_ = 1.0f;
	Vector2 waveVelocity_ = {};

private:

	std::unique_ptr<Sprite> backGroundSprite_;
	std::unique_ptr<Sprite> textSprite_;
	std::unique_ptr<Sprite> ghostSprite_;
	std::unique_ptr<Sprite> AButtonSprite_;

	Vector2 textPosition_ = {};
	Vector2 ghostPosition_ = {};
	Vector2 AButtonPosition_ = {};

};

