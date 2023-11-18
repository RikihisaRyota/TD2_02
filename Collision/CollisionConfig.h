#pragma once

#include <cstdint>

// プレイヤー
const uint32_t kCollisionAttributePlayer = 0b1;
// ブロック
const uint32_t kCollisionAttributeBlock = 0b1 << 1;
// ゴール
const uint32_t kCollisionAttributeGoal = 0b1 << 2;