#pragma once

class StageData {
public:
	enum Stage {
		kStage_1,
		kStage_2,
		kStage_3,
		kStage_4,
		kStage_5,
		kStage_6,
		kStage_7,
		kStage_8,

		kStageCount,
	};

	struct Data {
		// フレーム数
		int time;
		int itemCount;
		bool isClear;
	};
public:
	StageData();

	static void SetData(int time,int itemCount,bool flag,int currentStage);
	static Data GetData(int currentStage) { return data_[currentStage]; }

	static int GetClearTime(int currentStage) {return data_[currentStage].time;}
	static int GetClearItemCount(int currentStage) {return data_[currentStage].itemCount;}
	static bool GetClearFlag(int currentStage) {return data_[currentStage].isClear;}
	static void SetClearTime(int time,int currentStage) { data_[currentStage].time = time; }
	static void SetClearItemCount(int itemCount, int currentStage) { data_[currentStage].itemCount = itemCount; }
	static void SetClearFlag(bool flag, int currentStage) { data_[currentStage].isClear =flag; }
private:
	static Data data_[kStageCount];
};