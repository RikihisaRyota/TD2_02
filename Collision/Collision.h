#pragma once
#include <vector>
#include "WorldTransform.h"
#include "MapChip.h"

bool IsCollisionXY(const WorldTransform& transform1, const WorldTransform& transform2);

void CollisionEdit(const std::vector<std::vector<WorldTransform>>& mapchip, const std::vector<std::vector<uint32_t>>& type, WorldTransform* transform, Vector3* velocity);