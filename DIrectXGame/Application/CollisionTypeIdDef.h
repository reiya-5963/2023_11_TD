#pragma once

#include <cstdint>

// Collision種別ID定義
enum class CollisionTypeIdDef : uint32_t {
	kDefault,
	kPlayer1,
	kPlayer2,
	kMoveGimmick,
	kGoal,
};