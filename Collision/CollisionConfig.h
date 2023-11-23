#pragma once

#include <cstdint>

// プレイヤー
const uint32_t kCollisionAttributePlayer = 0b1;
// ブロック
const uint32_t kCollisionAttributeBlock = 0b1 << 1;
// ゴール
const uint32_t kCollisionAttributeGoal = 0b1 << 2;
// 当たったらアウト
const uint32_t kCollisionAttributeOut = 0b1 << 3;
// 当たったらアイテム
const uint32_t kCollisionAttributeItem = 0b1 << 4;