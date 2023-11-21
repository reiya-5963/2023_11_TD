#pragma once
#include "viewProjection.h"
#include "GlobalVariables.h"
#include "Vector3.h"

class ICamera
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize() {};
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() {};
	/// <summary>
	/// json保存用
	/// </summary>
	virtual void ApplyGlobalVariables() {};

protected:
	/// <summary>
	/// ビュー
	/// </summary>
	ViewProjection viewProjection;

	/// <summary>
	/// カメラアニメーション用構造体
	/// </summary>
	struct CameraAnimation {
		Vector3 startPoint_;
		Vector3 endPoint_;
		uint32_t frame_;
		float ease_t_;
	};

public: // アクセッサ
	/// <summary>
	/// ビュー
	/// </summary>
	/// <returns></returns>
	ViewProjection GetView() { return viewProjection; }
	/// <summary>
	/// 座標
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector3& position) { viewProjection.translation_ = position; }
	/// <summary>
	/// 回転角
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& rotation) { viewProjection.rotation_ = rotation; }
	/// <summary>
	/// スケール
	/// </summary>
	/// <param name="scale"></param>
	void SetScale(const Vector3& scale) { viewProjection.scale_ = scale; }

};

