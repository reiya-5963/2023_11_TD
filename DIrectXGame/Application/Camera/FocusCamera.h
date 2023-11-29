#pragma once
#include "ICamera.h"
#include "WorldTransform.h"

class FocusCamera : public ICamera
{
public:
	enum CameraPoint {
		kLeftBottom,
		kMiddleBottom,
		kRightBottom,
		kLeftTop,
		kMiddleTop,
		kRightTop,
	};

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

	void SetPlayerWorld2ScreenPos(Vector3& worldPos1, Vector3& worldPos2);
	void SetIsControlPlayer1(bool isControl) { isControlPlayer1 = isControl; }
	//Vector2 GetPlayerScreenPos() { return playerScreenPos_; }

	void SetIsEase(bool isEase) { isEase_ = isEase; }
	bool IsEase() { return isEase_; }
	bool IsControlPlayer1() { return isControlPlayer1; }
	bool IsPreControlPlayer1() { return isPreControlPlayer1; }

	std::vector<Vector3> GetCenters() { return centers_; }
	void SetCameraPoint(uint32_t point) { cameraPoint_ = point; }

private:
	CameraAnimation animater_;
	Vector3 startRot_ = {};
	Vector3 endRot_ = {};
	bool isEase_ = false;

	// ターゲットになるワールド変換データ
	const WorldTransform* target_ = nullptr;

private:
	std::vector<Vector3>centers_;

	

	uint32_t cameraPoint_ = kLeftBottom;


	Vector2 player1ScreenPos_ = {};
	Vector2 player2ScreenPos_ = {};
	bool isControlPlayer1 = true;
	bool isPreControlPlayer1 = isControlPlayer1;
	bool isMoveMap_ = false;
	bool isTogether_ = false;
};

