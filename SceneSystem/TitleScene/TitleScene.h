#pragma once

#include "SceneSystem/IScene/IScene.h"
#include "Game/TitleSprites/TitleSprites.h"

class TitleScene : public IScene
{
public:
	TitleScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

private:

	std::unique_ptr<TitleSprites> titleSprites_;
	// サウンド
	int32_t choiceSoundHandle_;
};

