#pragma once

#include <string>

#include "Vector2.h"

class StageData {
public:
	static const int kMaxCondition = 3;
	enum V2ItemNames {
		kConditionTime,
		kV2ItemCount,
	};

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
		// クリア条件のタイム
		int conditionTime;
		// クリア時のタイム
		int clearTime;
		// クリア条件のアイテム数
		int conditionItemCount;
		// クリア時のアイテム数
		int clearItemCount;
		bool isClear;
	};
public:
	StageData();
	static void Initialize();
	static void Update();

	static void SetData(int time,int itemCount,bool flag,int currentStage);
	static Data GetData(int currentStage) { return data_[currentStage]; }

	static int GetClearTime(int currentStage) {return data_[currentStage].clearTime;}
	static int GetConditionTime(int currentStage) {return data_[currentStage].conditionTime;}

	static int GetClearItemCount(int currentStage) {return data_[currentStage].clearItemCount;}
	static int GetConditionItemCount(int currentStage) {return data_[currentStage].conditionItemCount;}
	
	static bool GetClearFlag(int currentStage) {return data_[currentStage].isClear;}
	
	static void SetConditionTime(int conditionTime,int currentStage) { data_[currentStage].conditionTime = conditionTime; }
	static void SetClearTime(int clearTime, int currentStage) { data_[currentStage].clearTime = clearTime; }

	static void SetConditionItemCount(int conditionItemCount, int currentStage) { data_[currentStage].conditionItemCount = conditionItemCount; }
	static void SetClearItemCount(int clearItemCount, int currentStage) { data_[currentStage].clearItemCount = clearItemCount; }
	
	static void SetClearFlag(bool flag, int currentStage) { data_[currentStage].isClear =flag; }
private:
	static void SetGlobalVariable();
	static void ApplyGlobalVariable();
	static Data data_[kStageCount];

	static std::string stageNames_[Stage::kStageCount];

	static std::string v2ItemNames_[V2ItemNames::kV2ItemCount];

	static Vector2 v2Info_[Stage::kStageCount][V2ItemNames::kV2ItemCount];

	static std::string groupName_;
};