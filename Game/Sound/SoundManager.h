#pragma once

#include <vector>

#include "Audio.h"

class SoundManager {
public:
	enum Scene {
		kTitleScene,
		kSelectScene,
		kStageScene,
		kClearScene,

		kCount,
	};

	SoundManager();

	void Initialize();
	void Update();

	void SetScene(int currentScene, int preScene);
private:
	Audio* audio_;
	std::vector<int32_t> soundHandle_;
	std::vector<int32_t> playHandle_;
	bool isLoop_;
	int preScene_;
	int currentScene_;
};