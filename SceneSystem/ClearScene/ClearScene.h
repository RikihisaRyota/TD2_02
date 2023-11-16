#pragma once

#include "SceneSystem/IScene/IScene.h"

class ClearScene : public IScene
{
public:
	ClearScene();

	void Init() override;
	void Update() override;
	void Draw() override;
};
