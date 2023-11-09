#include "Collision.h"
#include "MyMath.h"

bool IsCollisionXY(const WorldTransform& transform1, const WorldTransform& transform2)
{
	Vector2 min0 = { transform1.worldPos_.x - transform1.scale_.x,transform1.worldPos_.y - transform1.scale_.y };
	Vector2 max0 = { transform1.worldPos_.x + transform1.scale_.x,transform1.worldPos_.y + transform1.scale_.y };

	Vector2 min1 = { transform2.worldPos_.x - transform2.scale_.x,transform2.worldPos_.y - transform2.scale_.y };
	Vector2 max1 = { transform2.worldPos_.x + transform2.scale_.x,transform2.worldPos_.y + transform2.scale_.y };

	if (min0.x <= max1.x && max0.x >= min1.x &&
		min0.y <= max1.y && max0.y >= min1.y) {
		return true;
	}

	return false;
}

void CollisionEdit(const std::vector<std::vector<WorldTransform>>& mapchip, const std::vector<std::vector<uint32_t>>& type, WorldTransform* transform, Vector3* velocity)
{

	bool isJump = false;

	if (velocity->x != 0 || velocity->y != 0) {

		WorldTransform sub = *transform;
		bool finishFlag = false;

		for (int y = 0; y < int(mapchip.size()); y++) {
			for (int x = 0; x < int(mapchip[y].size()); x++) {

				if (IsCollisionXY(sub,mapchip[y][x])) {

					int nextY = y;
					int nextX = x;

					switch (type[y][x])
					{
					case 0:
						// あたり判定なし
						break;
					case 1:
						// 通れないブロックの場合

						if (velocity->x == 0 && velocity->y != 0) {
							// x軸方向の移動がない場合
							if (velocity->y > 0) {
								sub.translate_.y = mapchip[y][x].worldPos_.y - mapchip[y][x].scale_.y - sub.scale_.y - 0.001f;
							}
							else {
								sub.translate_.y = mapchip[y][x].worldPos_.y + mapchip[y][x].scale_.y + sub.scale_.y + 0.001f;
							}
							velocity->y = 0.0f;
							finishFlag = true;
							break;
						}
						else if (velocity->y == 0 && velocity->x != 0) {
							// y軸方向の移動がない場合
							if (velocity->x > 0) {
								sub.translate_.x = mapchip[y][x].worldPos_.x - mapchip[y][x].scale_.x - sub.scale_.x - 0.001f;
							}
							else {
								sub.translate_.x = mapchip[y][x].worldPos_.x + mapchip[y][x].scale_.x + sub.scale_.x + 0.001f;
							}

							finishFlag = true;
							break;
						}
						else {
							// 他のすべての場合
							bool isFirstX = false;

							Vector2 beforePos = { sub.translate_.x - velocity->x , sub.translate_.y - velocity->y };
							Vector2 chipPos = { mapchip[y][x].worldPos_.x,mapchip[y][x].worldPos_.y };

							if (velocity->x > 0) {
								// xが正の方向に移動する場合
								if (velocity->y < 0) {
									// yが負の方向に移動する場合
									if (beforePos.x < chipPos.x - mapchip[y][x].scale_.x - sub.scale_.x) {
										if (beforePos.y < chipPos.y + mapchip[y][x].scale_.y + sub.scale_.y) {
											isFirstX = true;
										}
										else {
											if (Outer({ (chipPos.x - mapchip[y][x].scale_.x) - (beforePos.x + sub.scale_.x), (chipPos.y + mapchip[y][x].scale_.y) - (beforePos.y - sub.scale_.y) },
												{ velocity->x,velocity->y }) > 0) {
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

										sub.translate_.x = chipPos.x - mapchip[y][x].scale_.x - sub.scale_.x - 0.001f;

										sub.UpdateMatrix();
										nextX -= 1;

										if (y == int(mapchip.size())) {
											switch (type[y][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[y][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = chipPos.y + mapchip[y][nextX].scale_.y + sub.scale_.y + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextY += 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = mapchip[nextY][nextX].worldPos_.y + mapchip[nextY][nextX].scale_.y + sub.scale_.y + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}

									}
									else {
										// y軸から修正する

										velocity->y = 0.0f;
										sub.translate_.y = chipPos.y + mapchip[y][nextX].scale_.y + sub.scale_.y + 0.001f;

										sub.UpdateMatrix();
										nextY -= 1;

										if (x == int(mapchip.size())) {
											switch (type[nextY][x]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][x])) {
													sub.translate_.x = chipPos.x - mapchip[nextY][x].scale_.x - sub.scale_.x - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextX += 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													sub.translate_.x = chipPos.x - mapchip[nextY][nextX].scale_.x - sub.scale_.x - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
									}

									finishFlag = true;
								}
								else {
									// yが正の方向に移動する場合
									if (beforePos.x < chipPos.x - mapchip[y][x].scale_.x - sub.scale_.x) {
										if (beforePos.y > chipPos.y - mapchip[y][x].scale_.y - sub.scale_.y) {
											isFirstX = true;
										}
										else {
											if (Outer({ (chipPos.x - mapchip[y][x].scale_.x) - (beforePos.x + sub.scale_.x), (chipPos.y - mapchip[y][x].scale_.y) - (beforePos.y + sub.scale_.y) },
												{ velocity->x,velocity->y }) > 0) {
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

										sub.translate_.x = chipPos.x - mapchip[y][x].scale_.x - sub.scale_.x - 0.001f;

										sub.UpdateMatrix();
										nextX -= 1;

										if (y == 0) {
											switch (type[y][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[y][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = chipPos.y - mapchip[y][nextX].scale_.y - sub.scale_.y - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextY -= 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = mapchip[nextY][nextX].worldPos_.y - mapchip[nextY][nextX].scale_.y - sub.scale_.y - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}

									}
									else {
										// y軸から修正する

										velocity->y = 0.0f;
										sub.translate_.y = chipPos.y - mapchip[y][nextX].scale_.y - sub.scale_.y - 0.001f;

										sub.UpdateMatrix();
										nextY += 1;

										if (x == int(mapchip.size())) {
											switch (type[nextY][x]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][x])) {
													sub.translate_.x = chipPos.x - mapchip[nextY][x].scale_.x - sub.scale_.x - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextX += 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													sub.translate_.x = mapchip[nextY][nextX].worldPos_.x - mapchip[nextY][nextX].scale_.x - sub.scale_.x - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
									}

									finishFlag = true;

								}
							}
							else {
								// xが負の方向に移動する場合

								if (velocity->y < 0) {
									// yが負の方向に移動する場合
									if (beforePos.x > chipPos.x + mapchip[y][x].scale_.x + sub.scale_.x) {
										if (beforePos.y < chipPos.y + mapchip[y][x].scale_.y + sub.scale_.y) {
											isFirstX = true;
										}
										else {
											if (Outer({ (chipPos.x + mapchip[y][x].scale_.x) - (beforePos.x - sub.scale_.x), (chipPos.y + mapchip[y][x].scale_.y) - (beforePos.y - sub.scale_.y) },
												{ velocity->x,velocity->y }) < 0) {
												isFirstX = false;
											}
											else {

												if (IsCollisionXY(sub, mapchip[y][x + 1])) {
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

										sub.translate_.x = chipPos.x + mapchip[y][x].scale_.x + sub.scale_.x + 0.001f;

										sub.UpdateMatrix();
										nextX += 1;

										if (y == int(mapchip.size())) {
											switch (type[y][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[y][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = chipPos.y + mapchip[y][nextX].scale_.y + sub.scale_.y + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextY += 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = mapchip[nextY][nextX].worldPos_.y + mapchip[nextY][nextX].scale_.y + sub.scale_.y + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}

									}
									else {
										// y軸から修正する

										velocity->y = 0.0f;
										sub.translate_.y = chipPos.y + mapchip[y][nextX].scale_.y + sub.scale_.y + 0.001f;

										sub.UpdateMatrix();
										nextY -= 1;

										if (x == 0) {
											switch (type[nextY][x]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][x])) {
													sub.translate_.x = chipPos.x + mapchip[nextY][x].scale_.x + sub.scale_.x + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextX -= 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													sub.translate_.x = mapchip[nextY][nextX].worldPos_.x + mapchip[nextY][nextX].scale_.x + sub.scale_.x + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
									}

									finishFlag = true;

								}
								else {
									// yが正の方向に移動する場合
									if (beforePos.x > chipPos.x + mapchip[y][x].scale_.x + sub.scale_.x) {
										if (beforePos.y > chipPos.y - mapchip[y][x].scale_.y - sub.scale_.y) {
											isFirstX = true;
										}
										else {
											if (Outer({ (chipPos.x + mapchip[y][x].scale_.x) - (beforePos.x - sub.scale_.x), (chipPos.y - mapchip[y][x].scale_.y) - (beforePos.y + sub.scale_.y) },
												{ velocity->x,velocity->y }) < 0) {
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

										sub.translate_.x = chipPos.x + mapchip[y][x].scale_.x + sub.scale_.x + 0.001f;

										sub.UpdateMatrix();
										nextX += 1;

										if (y == 0) {
											switch (type[y][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[y][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = chipPos.y - mapchip[y][nextX].scale_.y - sub.scale_.y - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											//nextY -= 1;

											switch (type[nextY][nextX]) {
											case 0:

												nextY -= 1;

												switch (type[nextY][nextX]) {
												case 0:

													break;
												case 1:
													if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
														velocity->y = 0.0f;
														sub.translate_.y = mapchip[nextY][nextX].worldPos_.y - mapchip[nextY][nextX].scale_.y - sub.scale_.y - 0.001f;
													}
													break;
												case 2:
													isJump = true;

													break;

												default:
													break;
												}

												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													velocity->y = 0.0f;
													sub.translate_.y = mapchip[nextY][nextX].worldPos_.y - mapchip[nextY][nextX].scale_.y - sub.scale_.y - 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}

									}
									else {
										// y軸から修正する

										velocity->y = 0.0f;
										sub.translate_.y = chipPos.y - mapchip[y][nextX].scale_.y - sub.scale_.y - 0.001f;

										sub.UpdateMatrix();
										nextY += 1;

										if (x == 0) {
											switch (type[nextY][x]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][x])) {
													sub.translate_.x = chipPos.x + mapchip[nextY][x].scale_.x + sub.scale_.x + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
										else {
											nextX -= 1;

											switch (type[nextY][nextX]) {
											case 0:
												break;
											case 1:
												if (IsCollisionXY(sub, mapchip[nextY][nextX])) {
													sub.translate_.x = mapchip[nextY][nextX].worldPos_.x + mapchip[nextY][nextX].scale_.x + sub.scale_.x + 0.001f;
												}
												break;
											case 2:
												isJump = true;

												break;

											default:
												break;
											}
										}
									}

									finishFlag = true;

								}
							}

						}

						break;

					case 2:
						// 跳ねるブロックの場合
						isJump = true;
						break;

					default:
						break;
					}
				}

				if (finishFlag) {
					break;
				}
			}

			if (finishFlag) {
				break;
			}
		}

		sub.UpdateMatrix();
		*transform = sub;
	}



	// 跳ねる時の処理。ブロックにめり込んでた場合の処理をした後にするためここにある。
	if (isJump) {

	}



}
