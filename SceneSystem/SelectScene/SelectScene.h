#pragma once

#include "SceneSystem/IScene/IScene.h"
#include "Game/SelectSprites/SelectSprites.h"

class SelectScene : public IScene
{
public:
	SelectScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

private:
	std::unique_ptr<SelectSprites> selectSprites_;
	int32_t returnTitleSoundHandle_;
	
};

