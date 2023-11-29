#pragma once

#include "Game/Background/Background.h"
#include "Game/Goal/Goal.h"
#include "SceneSystem/IScene/IScene.h"
#include "DebugCamera.h"
#include "Game/Player/Player.h"
#include "MapChip.h"
#include "Game/FollowCamera/FollowCamera.h"
#include "MapChipEditor.h"
#include "Game/Pause/Pause.h"
#include "Game/Timer/Timer.h"
#include "Game/StageUI/StageUI.h"
#include "Game/StageBanner/StageBanner.h"

class StageScene : public IScene
{
public:

	StageScene();

	void Init() override;
	void Update() override;
	void Draw() override;
	void UIDraw() override;

	void Reset();
private:

	std::unique_ptr<DebugCamera> debugCamera_;

	// デバック用
	bool isDebug_;
	/// <summary>
	/// ゲーム用
	/// </summary>
	std::unique_ptr<Background> background_;
	std::unique_ptr<Goal> goal_;
	std::unique_ptr<MapChip> mapChip_;
	std::unique_ptr<MapChipEditor> mapChipEditor_;
	std::unique_ptr<FollowCamera> followCamera_;
	std::unique_ptr<Player> player_;
	std::unique_ptr<Pause> pause_;
	std::unique_ptr<Timer> timer_;
	std::unique_ptr<StageUI> stageUi_;
	std::unique_ptr<StageBanner> stageBanner_;
};
