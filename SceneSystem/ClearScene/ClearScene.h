#pragma once
#include <memory>

#include "SceneSystem/IScene/IScene.h"

#include "Game/ClearSprites/ClearSprites.h"

class ClearScene : public IScene
{
public:
	ClearScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;
private:
	std::unique_ptr<ClearSprites> clearSprites_;

};
