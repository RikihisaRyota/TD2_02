#pragma once

#include <memory>
#include <optional>

#include "Sprite.h"
#include "Vector4.h"

class SceneChange {
public:
	SceneChange();
	void Initialize();
	void Update();
	void Draw();

	bool GetInitialize() { return canInitialize_; }
	void SetInitialize(bool flag) { canInitialize_ = flag; }
	bool GetSceneChange() { return isSceneChange_; }
	void SetSceneChange(bool flag);
private:
	enum State {
		kNone,
		kIn,
		kOut,

		kCount,
	};

	static void (SceneChange::* spStateInitFuncTable[])();
	static void (SceneChange::* spStateUpdateFuncTable[])();

	void StateRequest(State state);

	void NoneInitialize();
	void NoneUpdate();

	void InInitialize();
	void InUpdate();

	void OutInitialize();
	void OutUpdate();
	
	bool isSceneChange_;
	bool canInitialize_;
	float time_;
	float maxTime_;

	std::unique_ptr<Sprite> background_;
	Vector4 startColor_;
	Vector4 endColor_;
	Vector4 color_;

	// 今の状態
	State state_ = State::kIn;
	State preState_ = State::kIn;
	// 状態のリクエスト
	std::optional<State> stateRequest_ = std::nullopt;
};

