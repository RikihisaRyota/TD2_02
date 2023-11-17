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

	void Initialize(int currentScene, int preScene);
	void Update();
private:
	Audio* audio_;
	std::vector<int32_t> soundHandle_;
	std::vector<int32_t> playHandle_;
	bool isLoop_;
	int preScene_;
	int currentScene_;
};