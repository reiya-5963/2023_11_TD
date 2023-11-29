#pragma once
#include "ICamera.h"
#include "WorldTransform.h"

class FocusCamera : public ICamera
{
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;
	/// <summary>
	/// json保存用
	/// </summary>
	void ApplyGlobalVariables() override;

	/// <summary>
	/// イージングする際の座標設定
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	void SettingAnimation(const Vector3& start, const Vector3& end);
	
	void SetIsAnimater(bool isEase) { isEase_ = isEase; }

	void SetParent(WorldTransform* parent) { target_ = parent; }

private:
	CameraAnimation animater_;
	Vector3 startRot_ = {};
	Vector3 endRot_ = {};
	bool isEase_ = false;

	// ターゲットになるワールド変換データ
	const WorldTransform* target_ = nullptr;

private:

	std::vector<Vector3>centers_;

	enum CameraPoint {
		kLeftBottom,
		kRightBottom,
		kLeftTop,
		kRightTop,
	};

	uint32_t cameraPoint_ = kLeftBottom;

};

