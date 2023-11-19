#pragma once

#include <memory>
#include <vector>

#include "Sprite.h"
#include "Random.h"
#include "WorldTransform.h"
#include "Vector4.h"
#include "ViewProjection.h"

class Background {
public:
	Background();
	void Initialize();
	void Update();
	void SpriteDraw();
	void Draw(const ViewProjection& viewProjection);
private:
	std::unique_ptr<Sprite> sprite_;
	Vector4 color_;
	WorldTransform worldTransform_;
	int time_;
	Random::RandomNumberGenerator rnd_;
};

