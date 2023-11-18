#pragma once

#include "Game/Background/Background.h"
#include "SceneSystem/IScene/IScene.h"
#include "DebugCamera.h"
#include "Game/Player/Player.h"
#include "MapChip.h"
#include "Game/FollowCamera/FollowCamera.h"
#include "MapChipEditor.h"

class StageScene : public IScene
{
public:

	StageScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

private:

	std::unique_ptr<DebugCamera> debugCamera_;

	// デバック用
	bool isDebug_;
	/// <summary>
	/// ゲーム用
	/// </summary>
	std::unique_ptr<Background> background_;
	std::unique_ptr<MapChip> mapChip_;
	std::unique_ptr<MapChipEditor> mapChipEditor_;

	std::unique_ptr<FollowCamera> followCamera_;

	std::unique_ptr<Player> player_;
};
