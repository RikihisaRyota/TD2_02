#pragma once

#include "SceneSystem/IScene/IScene.h"

class SelectScene : public IScene
{
public:
	SelectScene();

	void Init() override;
	void Update() override;
	void Draw() override;

private:

};

