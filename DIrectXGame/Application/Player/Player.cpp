#include "Player.h"
#include "Input.h"
#include "R_Math.h"

void Player::Initialize(Model* model)
{
	// モデルの設定
	model_ = model;
	// 初期化
	worldTransform_.Initialize();
}

void Player::Setting(const Vector3& position, uint32_t color)
{
	// 座標
	worldTransform_.translation_ = position;
	// 色
	color;
}

void Player::Update()
{
	state_->Update();

	worldTransform_.UpdateMatrix();
}

void Player::Draw(ViewProjection& viewProjection)
{
	model_->Draw(worldTransform_, viewProjection);
}

void Player::MoveUpdate(Vector3& moveDirect)
{
	worldTransform_.translation_ = R_Math::Add(worldTransform_.translation_, moveDirect);
}

void Player::SetState(PlayerState* state)
{
	state_ = state;
	state_->SetPlayer(this);
}
