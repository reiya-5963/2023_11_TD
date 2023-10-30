#pragma once

#include <cstdint>

// 味方陣営 (0000 0001)
const uint32_t kCollisionAttributePlayer = 0b1;

// 敵陣営 (0000 0010)
const uint32_t kCollisionAttributeEnemy = 0b1 << 1;

// 地面 (0000 0100)
const uint32_t kCollisionAttributeWorld = 0b1 << 2;
