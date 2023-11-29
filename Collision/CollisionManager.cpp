#include "CollisionManager.h"
#include "ColliderShapes/ColliderShapeBox2D.h"
#include "ColliderShapes/ColliderShapeMapChip2D.h"
#include "MyMath.h"
#include <functional>

CollisionManager* CollisionManager::GetInstance()
{
	static CollisionManager instance;
	return &instance;
}

void CollisionManager::Init()
{
	Clear();
}

void CollisionManager::Clear()
{
	colliders_.clear();
}

void CollisionManager::SetCollider(Collider* collider)
{
	colliders_.push_back(collider);
}

void CollisionManager::CheckCollision()
{

	std::list<Collider*>::iterator itrA = colliders_.begin();
	for (; itrA != colliders_.end(); itrA++) {
		Collider* colliderA = *itrA;

		std::list<Collider*>::iterator itrB = itrA;
		itrB++;

		for (; itrB != colliders_.end(); itrB++) {
			Collider* colliderB = *itrB;

			if (IsNeedCollision(colliderA, colliderB) && IsMatchedConfig(colliderA, colliderB)) {

				if (IsCollision(colliderA,colliderB)) {

					colliderA->editInfo_.colliderTypeMask_ = colliderB->shapeType_->colliderType_;
					colliderB->editInfo_.colliderTypeMask_ = colliderA->shapeType_->colliderType_;

					colliderA->OnCollision();

					colliderB->OnCollision();

				}
			}
		}

	}

}

bool CollisionManager::IsNeedCollision(Collider* a, Collider* b) const
{
	if (a->shapeType_->GetColliderType() == BaseColliderShapeType::ColliderType::UNKNOWN ||
		b->shapeType_->GetColliderType() == BaseColliderShapeType::ColliderType::UNKNOWN) {
		return false;
	}

	return true;
}

bool CollisionManager::IsMatchedConfig(Collider* a, Collider* b) const
{

	if (((a->GetCollisionAttribute() & b->GetCollisionMask()) >= 0b1) ||
		((b->GetCollisionAttribute() & a->GetCollisionMask()) >= 0b1)) {
		return true;
	}

	return false;
}

bool CollisionManager::IsCollision(Collider* a, Collider* b) const
{

	if (a->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::BOX2D && b->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::BOX2D) {

		if (IsCollisionBox2DBox2D(a, b)) {
			return true;
		}
		
		return false;
	}

	if (a->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::BOX2D && b->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::MAPCHIP2D) {
		if (IsCollisionBox2DMapChip2D(a, b)) {
			return true;
		}

		return false;
	}

	if (b->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::BOX2D && a->shapeType_->shapeType_ == BaseColliderShapeType::ShapeType::MAPCHIP2D) {
		if (IsCollisionBox2DMapChip2D(b, a)) {
			return true;
		}

		return false;
	}

	return false;
}

bool CollisionManager::IsCollisionBox2DBox2D(Collider* a, Collider* b) const
{

	if (ColliderShapeBox2D::IsCollisionBox2DBox2D(a->shapeType_.get(), b->shapeType_.get())) {
		a->editInfo_.collisionMask_ = b->GetCollisionAttribute();
		b->editInfo_.collisionMask_ = a->GetCollisionAttribute();
		a->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2MASKPOS] = b->shapeType_->v2Paras_[ColliderShapeBox2D::v2Info::CENTER];
		b->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2MASKPOS] = a->shapeType_->v2Paras_[ColliderShapeBox2D::v2Info::CENTER];
		return true;
	}

	return false;
}

bool CollisionManager::IsCollisionBox2DMapChip2D(Collider* a, Collider* b) const
{

	Vector2 velocity = a->shapeType_->v2Paras_[ColliderShapeBox2D::v2Info::VELOCITY];

	bool result = false;
	
	Vector2 worldPos = a->shapeType_->v2Paras_[ColliderShapeBox2D::v2Info::CENTER];
	Vector2 scale = a->shapeType_->v2Paras_[ColliderShapeBox2D::v2Info::SCALE];

	Vector2 mapChipScale = b->shapeType_->v2Paras_[ColliderShapeMapChip2D::v2Info::CHIPSIZE];
	Vector2 bottomLeft = b->shapeType_->v2Paras_[ColliderShapeMapChip2D::v2Info::BOTTOMLEFTPOS];

	Vector2 sub = worldPos;

	a->editInfo_.i32Info_.clear();
	b->editInfo_.pairIInfo_.clear();

	if (velocity.x != 0 || velocity.y != 0) {

		bool finishFlag = false;

		float edit = 0.001f;

		int y = b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - static_cast<int>((worldPos.y + scale.y - bottomLeft.y) / (mapChipScale.y * 2));
		int x = static_cast<int>((worldPos.x -scale.x - bottomLeft.x) / (mapChipScale.x * 2));

		int endY = b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - static_cast<int>((worldPos.y - scale.y - bottomLeft.y) / (mapChipScale.y * 2));
		if (endY >= b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM]) {
			endY = b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - 1;
		}
		int endX = static_cast<int>((worldPos.x + scale.x - bottomLeft.x) / (mapChipScale.x * 2));

		std::function<void()> editXY = [&]() {
			if (y < 0) {
				y = 0;
			}
			else if (y >= int(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM])){
				y = int(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM]) - 1;
			}

			if (x < 0) {
				x = 0;
			}
			else if (x >= int(b->shapeType_->mapChip2D_.mapChip_[y].size())) {
				x = int(b->shapeType_->mapChip2D_.mapChip_[y].size()) - 1;
			}
		};

		for (int row = y; row <= endY; row++) {
			for (int column = x; column <= endX; column++) {

	
				int nextY = row;
				int nextX = column;

				std::function<void()> editNextXY = [&]() {
					if (nextY < 0) {
						nextY = 0;
					}
					else if (nextY >= int(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM])) {
						nextY = int(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM]) - 1;
					}

					if (nextX < 0) {
						nextX = 0;
					}
					else if (nextX >= int(b->shapeType_->mapChip2D_.mapChip_[nextY].size())) {
						nextX = int(b->shapeType_->mapChip2D_.mapChip_[nextY].size()) - 1;
					}
				};

				if (b->shapeType_->mapChip2D_.IsCollider(row, column)) {
					result = true;
					// あたり判定があり

					a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
					b->editInfo_.SetPairIInfo(std::pair(row, column));

				}
				else if (b->shapeType_->mapChip2D_.IsRigitBody(row, column)) {
					result = true;
					// めり込み処理する

					Vector2 chipPos = { column * mapChipScale.x * 2 + mapChipScale.x + bottomLeft.x,
						(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - row) * mapChipScale.y * 2 + mapChipScale.y };

					if (velocity.x == 0 && velocity.y != 0) {
						// x軸方向の移動がない場合
						if (velocity.y > 0) {
							sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
						}
						else {
							sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
						}
						if (velocity.y >= 0.0f) {
							velocity.y = -edit;
						}
						else {
							velocity.y = 0.0f;
						}

						a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
						b->editInfo_.SetPairIInfo(std::pair(row, column));
						finishFlag = true;
						break;
					}
					else if (velocity.y == 0 && velocity.x != 0) {
						// y軸方向の移動がない場合
						if (velocity.x > 0) {
							sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
						}
						else {
							sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
						}
						velocity.x = 0.0f;
						a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
						b->editInfo_.SetPairIInfo(std::pair(row, column));
						finishFlag = true;
						break;
					}
					else {
						// 他のすべての場合
						bool isFirstX = false;

						Vector2 beforePos = { sub.x - velocity.x , sub.y - velocity.y };

						if (velocity.x > 0) {
							// xが正の方向に移動する場合
							if (velocity.y < 0) {
								// yが負の方向に移動する場合
								if (beforePos.x <= chipPos.x - mapChipScale.x - scale.x) {
									if (beforePos.y <= chipPos.y + mapChipScale.y + scale.y) {
										isFirstX = true;
									}
									else {
										if (Outer({ (chipPos.x - mapChipScale.x) - (beforePos.x + scale.x), (chipPos.y + mapChipScale.y) - (beforePos.y - scale.y) },
											{ velocity.x,velocity.y }) > -0.01f) {
											isFirstX = false;
										}
										else {
											isFirstX = true;
										}
									}
								}
								else {
									isFirstX = false;
								}

								if (isFirstX) {
									// x軸から先に修正する

									sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
									velocity.x = 0.0f;

									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
									b->editInfo_.SetPairIInfo(std::pair(nextY, column));

									//sub.UpdateMatrix();
									nextX -= 1;

									chipPos.x -= mapChipScale.x * 2;

									if (nextY == int(b->shapeType_->mapChip2D_.mapChip_.size()) - 1) {

										if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}
										else if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = 0.0f;
												sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}

									}
									else {
										nextY += 1;

										chipPos.y -= mapChipScale.y * 2;

										if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}
										else if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = 0.0f;
												sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}

									}

								}
								else {
									// y軸から修正する

									velocity.y = 0.0f;
									sub.y = chipPos.y + mapChipScale.y + scale.y + edit;

									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
									b->editInfo_.SetPairIInfo(std::pair(row, column));

									//sub.UpdateMatrix();
									nextY -= 1;

									chipPos.y += mapChipScale.x * 2;

									if (column == int(b->shapeType_->mapChip2D_.mapChip_[nextY].size()) - 1) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, column)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, column));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, column)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, column));
										}

									}
									else {
										nextX += 1;
										chipPos.x += mapChipScale.x * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}
								}

								finishFlag = true;
							}
							else {
								// yが正の方向に移動する場合
								if (beforePos.x <= chipPos.x - mapChipScale.x - scale.x) {
									if (beforePos.y >= chipPos.y - mapChipScale.y - scale.y) {
										isFirstX = true;
									}
									else {
										if (Outer({ (chipPos.x - mapChipScale.x) - (beforePos.x + scale.x), (chipPos.y - mapChipScale.y) - (beforePos.y + scale.y) },
											{ velocity.x,velocity.y }) >= -0.2f) {
											if (b->shapeType_->mapChip2D_.IsRigitBody(row + 1, column)) {
												isFirstX = true;
												nextY += 1;
												chipPos.y -= mapChipScale.y * 2;
											}
											else {
												isFirstX = false;
											}
										}
										else {
											// ここ計算的にはfalseだと思うんだけどな
											isFirstX = false;
										}
									}
								}
								else {
									isFirstX = false;
								}

								if (isFirstX) {
									// x軸から先に修正する

									sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
									velocity.x = 0.0f;

									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
									b->editInfo_.SetPairIInfo(std::pair(nextY, column));

									//sub.UpdateMatrix();
									nextX -= 1;

									chipPos.x -= mapChipScale.x * 2;

									if (nextY == 0) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = -edit;
												sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}
									else {
										nextY -= 1;

										chipPos.y += mapChipScale.y * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = -edit;
												sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}

								}
								else {
									// y軸から修正する

									velocity.y = -edit;
									sub.y = chipPos.y - mapChipScale.y - scale.y - edit;

									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
									b->editInfo_.SetPairIInfo(std::pair(row, column));

									//sub.UpdateMatrix();
									nextY += 1;
									chipPos.y -= mapChipScale.y * 2;

									if (column == int(b->shapeType_->mapChip2D_.mapChip_[nextY].size()) - 1) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, column)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, column));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, column)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][column]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, column));
										}

									}
									else {
										nextX += 1;
										chipPos.x += mapChipScale.x * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x - mapChipScale.x - scale.x - edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり
											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

										}

									}
								}

								finishFlag = true;

							}
						}
						else {
							// xが負の方向に移動する場合

							if (velocity.y < 0) {
								// yが負の方向に移動する場合
								if (beforePos.x >= chipPos.x + mapChipScale.x + scale.x) {
									if (beforePos.y <= chipPos.y + mapChipScale.y + scale.y) {
										isFirstX = true;
									}
									else {
										if (Outer({ (chipPos.x + mapChipScale.x) - (beforePos.x - scale.x), (chipPos.y + mapChipScale.y) - (beforePos.y - scale.y) },
											{ velocity.x,velocity.y }) < 0.01f) {
											isFirstX = false;
										}
										else {

											if (b->shapeType_->mapChip2D_.IsRigitBody(row, column + 1)) {
												isFirstX = false;
												nextX += 1;
												chipPos.x += mapChipScale.x * 2;
											}
											else {
												isFirstX = true;
											}
										}
									}
								}
								else {
									isFirstX = false;
								}

								if (isFirstX) {
									// x軸から先に修正する

									sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
									velocity.x = 0.0f;

									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
									b->editInfo_.SetPairIInfo(std::pair(row, column));

									//sub.UpdateMatrix();
									nextX += 1;
									chipPos.x += mapChipScale.x * 2;

									if (row == int(b->shapeType_->mapChip2D_.mapChip_.size()) - 1) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(row, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = 0.0f;
												sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(row, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(row, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(row, nextX));
										}

									}
									else {
										nextY += 1;
										chipPos.y -= mapChipScale.y * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = 0.0f;
												sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり
											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

										}
									}

								}
								else {
									// y軸から修正する

									velocity.y = 0.0f;
									sub.y = chipPos.y + mapChipScale.y + scale.y + edit;
									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][nextX]);
									b->editInfo_.SetPairIInfo(std::pair(row, nextX));

									//sub.UpdateMatrix();
									nextY -= 1;
									chipPos.y += mapChipScale.y * 2;

									if (nextX == 0) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}
									else {
										//nextX -= 1;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり
											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

										}
										else {

											nextX--;
											chipPos.x -= mapChipScale.x * 2;

											if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
												// あたり判定があり
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

											}
											else if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
												// めり込み処理する

												if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
													sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
													velocity.x = 0.0f;
													a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
													b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
												}
											}
										}


									}
								}

								finishFlag = true;

							}
							else {
								// yが正の方向に移動する場合
								if (beforePos.x >= chipPos.x + mapChipScale.x + scale.x) {
									if (beforePos.y >= chipPos.y - mapChipScale.y - scale.y) {
										isFirstX = true;
									}
									else {
										if (Outer({ (chipPos.x + mapChipScale.x) - (beforePos.x - scale.x), (chipPos.y - mapChipScale.y) - (beforePos.y + scale.y) },
											{ velocity.x,velocity.y }) <= 0.2f) {
											//isFirstX = true;
											if (b->shapeType_->mapChip2D_.IsRigitBody(row, column + 1)) {
												isFirstX = false;
												nextX += 1;
												chipPos.x += mapChipScale.x * 2;
											}
											else {
												isFirstX = true;
											}
											
										}
										else {
											// ここ計算的にはfalseだと思うんだけどな
											isFirstX = false;
										}
									}
								}
								else {
									isFirstX = false;
								}

								if (isFirstX) {
									// x軸から先に修正する

									sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
									velocity.x = 0.0f;
									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[row][column]);
									b->editInfo_.SetPairIInfo(std::pair(row, column));
									//sub.UpdateMatrix();
									nextX += 1;
									chipPos.x += mapChipScale.x * 2;

									if (row == 0) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = -edit;
												sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}
									else {
										//nextY -= 1;
										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = -edit;
												sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}
										else {

											nextY--;
											chipPos.y += mapChipScale.y * 2;

											if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
												// めり込み処理する

												if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
													velocity.y = -edit;
													sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
													a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
													b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

												}
											}
											else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
												// あたり判定があり
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));

											}
										}

									}

								}
								else {
									// y軸から修正する

									velocity.y = -edit;
									sub.y = chipPos.y - mapChipScale.y - scale.y - edit;
									a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
									b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
									//sub.UpdateMatrix();
									nextY += 1;
									chipPos.y -= mapChipScale.y * 2;

									if (nextX == 0) {

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}

									}
									else {
										nextX -= 1;
										chipPos.x -= mapChipScale.x * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x + mapChipScale.x + scale.x + edit;
												velocity.x = 0.0f;
												a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
												b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり

											a->editInfo_.SetI32Info(b->shapeType_->mapChip2D_.mapChip_[nextY][nextX]);
											b->editInfo_.SetPairIInfo(std::pair(nextY, nextX));
										}
									}
								}

								finishFlag = true;

							}
						}

					}

					break;

				}

				if (finishFlag) {
					break;
				}

			}

			if (finishFlag) {
				break;
			}
		}

		for (int i = 0; i < 4; i++) {

			/*if (i == 1) {
				x = static_cast<int>((worldPos.x + scale.x - bottomLeft.x) / (mapChipScale.x * 2));
			}
			else if (i == 2) {
				y = b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - static_cast<int>((worldPos.y - scale.y - bottomLeft.y) / (mapChipScale.y * 2));
				x = static_cast<int>((worldPos.x - scale.x - bottomLeft.x) / (mapChipScale.x * 2));
			}
			else if (i == 3) {
				x = static_cast<int>((worldPos.x + scale.x - bottomLeft.x) / (mapChipScale.x * 2));
			}*/

			//editXY();

			
		}
	}

	a->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS] = sub;
	a->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY] = velocity;

	a->editInfo_.collisionMask_ = b->GetCollisionAttribute();
	b->editInfo_.collisionMask_ = a->GetCollisionAttribute();

	if (a->editInfo_.i32Info_.size() > 0) {
		result = true;
	}

	return result;

}
