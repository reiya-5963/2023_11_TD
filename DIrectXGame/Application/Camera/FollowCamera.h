#pragma once
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "WinApp.h"

/// <summary>
/// 追従カメラ
/// </summary>
class FollowCamera {
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
	/// 追従対象の設定
	/// </summary>
	/// <param name="target">追従対象</param>
	void SetTarget(const WorldTransform* target);

	/// <summary>
	/// ビュープロジェクションの取得
	/// </summary>
	/// <returns>ビュープロジェクション</returns>
	ViewProjection& GetViewProjection();

private:
	// ビュープロジェクション
	ViewProjection viewProjection_;
	
	// fps視点でのカメラ位置
	Vector3 fpsView_ = { 0.0f, 4.0f, 0.0f };
	// tps視点でのカメラ位置
	Vector3 tpsView_ = { 0.0f, 4.0f, -30.0f };

	// ターゲットになるワールド変換データ
	const WorldTransform* target_ = nullptr;

	// マウスの位置
	POINT mousePos_;
	// 移動量を加算するためのやつ
	POINT preMousePos_;

	// 1フレームごとの移動量のためのやつ
	//float xMouseDistance = 0.0f;
	//float yMouseDistance = 0.0f;

	// キーマウ、コントローラーそれぞれの移動速度
	//float move_mouseSpeed = 0.002f;
	//float move_padSpeed = 5.0f;

	Vector3 offset_ = { 0.0f, 5.0f, 0.0f };

	//bool isDamage_ = false;

	//Vector3 playerRotation_{};

	// 浮遊動作の媒介変数
	//float damagingParameter_ = 0.0f;

	// 浮遊動作のサイクル
	//float damagingPeriod_ = 60.0f;

	// 浮遊動作の振れ幅
	//float damagingAmplitude_ = 60.0f;

	//
	bool isFps_ = false;

};