#pragma once

#include <vector>

#include "Model.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Background {
public:
	Background();
	void Initialize();
	void Update();
	void Draw(const ViewProjection& viewProjection);
private:
	std::unique_ptr<Model> model_;
	WorldTransform worldTransform_;
};

