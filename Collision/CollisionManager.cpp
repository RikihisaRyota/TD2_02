#include "CollisionManager.h"
#include "ColliderShapes/ColliderShapeBox2D.h"
#include "ColliderShapes/ColliderShapeMapChip2D.h"
#include "MyMath.h"

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

			if (IsMatchedConfig(colliderA, colliderB)) {

				if (IsCollision(colliderA,colliderB)) {

					colliderA->OnCollision();

					colliderB->OnCollision();

				}
			}
		}

	}

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

	if (velocity.x != 0 || velocity.y != 0) {

		bool finishFlag = false;

		int y = b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - static_cast<int>((worldPos.y + scale.y - bottomLeft.y) / (mapChipScale.y * 2));
		int x = static_cast<int>((worldPos.x -scale.x - bottomLeft.x) / (mapChipScale.x * 2));

		for (int i = 0; i < 4; i++) {

			if (i == 1) {
				x++;
			}
			else if (i == 2) {
				x--;
				y++;
			}
			else if (i == 3) {
				x++;
			}

			int nextY = y;
			int nextX = x;

			if (b->shapeType_->mapChip2D_.IsCollider(y, x)) {
				result = true;
				// あたり判定があり


			}
			else if (b->shapeType_->mapChip2D_.IsRigitBody(y, x)) {
				result = true;
				// めり込み処理する

				Vector2 chipPos = { x * mapChipScale.x * 2 + mapChipScale.x + bottomLeft.x,
					(b->shapeType_->iParas_[ColliderShapeMapChip2D::iInfo::MAXHEIGHTCHIPNUM] - y) * mapChipScale.y * 2 + mapChipScale.y };

				if (velocity.x == 0 && velocity.y != 0) {
					// x軸方向の移動がない場合
					if (velocity.y > 0) {
						sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
					}
					else {
						sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
					}
					if (velocity.y >= 0.0f) {
						velocity.y = -0.001f;
					}
					else {
						velocity.y = 0.0f;
					}
					finishFlag = true;
					break;
				}
				else if (velocity.y == 0 && velocity.x != 0) {
					// y軸方向の移動がない場合
					if (velocity.x > 0) {
						sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;
					}
					else {
						sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;
					}

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
							if (beforePos.x < chipPos.x - mapChipScale.x - scale.x) {
								if (beforePos.y < chipPos.y + mapChipScale.y + scale.y) {
									isFirstX = true;
								}
								else {
									if (Outer({ (chipPos.x - mapChipScale.x) - (beforePos.x + scale.x), (chipPos.y + mapChipScale.y) - (beforePos.y - scale.y) },
										{ velocity.x,velocity.y }) > 0) {
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

								sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;

								//sub.UpdateMatrix();
								nextX -= 1;

								chipPos.x -= mapChipScale.x * 2;

								if (y == int(b->shapeType_->mapChip2D_.mapChip_.size())) {

									if (b->shapeType_->mapChip2D_.IsCollider(y, nextX)) {
										// あたり判定があり


									}
									else if (b->shapeType_->mapChip2D_.IsRigitBody(y, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub,scale,chipPos,mapChipScale)) {
											velocity.y = 0.0f;
											sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
										}
									}

								}
								else {
									nextY += 1;

									chipPos.y -= mapChipScale.y * 2;

									if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
									else if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = 0.0f;
											sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
										}
									}

								}

							}
							else {
								// y軸から修正する

								velocity.y = 0.0f;
								sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;

								//sub.UpdateMatrix();
								nextY -= 1;

								chipPos.y += mapChipScale.x * 2;

								if (x == int(b->shapeType_->mapChip2D_.mapChip_[nextY].size())) {
									
									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, x)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, x)) {
										// あたり判定があり


									}
									
								}
								else {
									nextX += 1;
									chipPos.x += mapChipScale.x * 2;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}

								}
							}

							finishFlag = true;
						}
						else {
							// yが正の方向に移動する場合
							if (beforePos.x < chipPos.x - mapChipScale.x - scale.x) {
								if (beforePos.y > chipPos.y - mapChipScale.y - scale.y) {
									isFirstX = true;
								}
								else {
									if (Outer({ (chipPos.x - mapChipScale.x) - (beforePos.x + scale.x), (chipPos.y - mapChipScale.y) - (beforePos.y + scale.y) },
										{ velocity.x,velocity.y }) > 0) {
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

								sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;

								//sub.UpdateMatrix();
								nextX -= 1;

								chipPos.x -= mapChipScale.x * 2;

								if (y == 0) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(y, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = -0.001f;
											sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(y, nextX)) {
										// あたり判定があり


									}

								}
								else {
									nextY -= 1;

									chipPos.y += mapChipScale.y * 2;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = -0.001f;
											sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}

								}

							}
							else {
								// y軸から修正する

								velocity.y = -0.001f;
								sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;

								//sub.UpdateMatrix();
								nextY += 1;
								chipPos.y -= mapChipScale.y * 2;

								if (x == int(b->shapeType_->mapChip2D_.mapChip_[nextY].size())) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, x)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, x)) {
										// あたり判定があり


									}

								}
								else {
									nextX += 1;
									chipPos.x += mapChipScale.x * 2;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x - mapChipScale.x - scale.x - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


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
										{ velocity.x,velocity.y }) < 0) {
										isFirstX = false;
									}
									else {

										Vector2 hoge = chipPos;
										hoge.x += mapChipScale.x * 2;

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, hoge, mapChipScale)) {
											isFirstX = false;
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

								sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;

								//sub.UpdateMatrix();
								nextX += 1;
								chipPos.x += mapChipScale.x * 2;

								if (y == int(b->shapeType_->mapChip2D_.mapChip_.size())) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(y, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = 0.0f;
											sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(y, nextX)) {
										// あたり判定があり


									}

								}
								else {
									nextY += 1;
									chipPos.y -= mapChipScale.y * 2;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = 0.0f;
											sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
								}

							}
							else {
								// y軸から修正する

								velocity.y = 0.0f;
								sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;

								//sub.UpdateMatrix();
								nextY -= 1;
								chipPos.y += mapChipScale.y * 2;

								if (x == 0) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, x)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, x)) {
										// あたり判定があり


									}

								}
								else {
									//nextX -= 1;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = 0.0f;
											sub.y = chipPos.y + mapChipScale.y + scale.y + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
									else {

										nextX--;
										chipPos.x -= mapChipScale.x * 2;

										if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり


										}
										else if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;
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
										{ velocity.x,velocity.y }) < 0) {
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

								sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;

								//sub.UpdateMatrix();
								nextX += 1;
								chipPos.x += mapChipScale.x * 2;

								if (y == 0) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = -0.001f;
											sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
									
								}
								else {
									//nextY -= 1;
									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											velocity.y = -0.001f;
											sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
									else {

										nextY--;
										chipPos.y += mapChipScale.y * 2;

										if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
											// めり込み処理する

											if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
												velocity.y = -0.001f;
												sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;
											}
										}
										else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
											// あたり判定があり


										}
									}

								}

							}
							else {
								// y軸から修正する

								velocity.y = -0.001f;
								sub.y = chipPos.y - mapChipScale.y - scale.y - 0.001f;

								//sub.UpdateMatrix();
								nextY += 1;
								chipPos.y -= mapChipScale.y * 2;

								if (x == 0) {

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


									}
									
								}
								else {
									nextX -= 1;
									chipPos.x -= mapChipScale.x * 2;

									if (b->shapeType_->mapChip2D_.IsRigitBody(nextY, nextX)) {
										// めり込み処理する

										if (ColliderShapeBox2D::IsCollisionBox2DBox2D(sub, scale, chipPos, mapChipScale)) {
											sub.x = chipPos.x + mapChipScale.x + scale.x + 0.001f;
										}
									}
									else if (b->shapeType_->mapChip2D_.IsCollider(nextY, nextX)) {
										// あたり判定があり


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
	}

	a->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2POS] = sub;
	a->editInfo_.v2Paras_[Collider::EditInfo::EditEnumV2::V2VELOCITY] = velocity;

	return result;

}
