#pragma once
#include "ViewProjection.h"

enum SCENE { TITLE, SELECT, STAGE, CLEAR };

// シーン内での処理を行う基底クラス
class IScene
{
public:
	// シーン番号を管理する変数
	static int sceneNo_;

	// ステージ番号を管理する変数
	static int stageNo_;

	static const int MiniGameStageNo_ = 13;

public:
	// 継承先で実装される関数
	// 抽象クラスなので純粋仮想関数とする
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void UIDraw() = 0;

	virtual ~IScene();

	// シーン番号のゲッター
	int GetSceneNo();

protected:
	ViewProjection viewProjection_;
};

