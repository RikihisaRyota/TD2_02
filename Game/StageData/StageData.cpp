#include "StageData.h"

#include "GlobalVariables/GlobalVariables.h"

StageData::Data StageData::data_[MapChip::Stage::kCount];
StageData::Data StageData::bestData_[MapChip::Stage::kCount];

std::string StageData::stageNames_[MapChip::Stage::kCount] = {
		"ステージ1",
		"ステージ2",
		"ステージ3",
		"ステージ4",
		"ステージ5",
		"ステージ6",
		"ステージ7",
		"ステージ8",
		"ステージ9",
		"ステージ10",
		"ステージ11",
		"ステージ12",
		"ステージ13",
		"ステージ14",
		"ステージ15",
};
std::string StageData::v2ItemNames_[V2ItemNames::kV2ItemCount] = {
		"クリア条件タイム",
};
int StageData::v2Info_[MapChip::Stage::kCount][V2ItemNames::kV2ItemCount];
std::string StageData::groupName_ = "stageData";


StageData::StageData() {
	SetGlobalVariable();
	BestInitialize();
}

void StageData::Reset() {
	BestInitialize();
}

void StageData::Initialize() {
	SetGlobalVariable();
	BestInitialize();
}

void StageData::BestInitialize() {
	for (auto& data : bestData_) {
		data.clearTime = 9999;
		data.conditionTime = 9999;
		data.clearItemCount = -9999;
		data.conditionItemCount = 9999;
		data.isClear = false;
	}
}

void StageData::Update() {
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG
}

void StageData::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < MapChip::kCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, stageNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void StageData::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < MapChip::Stage::kCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetIntValue(groupName_, stageNames_[i] + v2ItemNames_[j]);
		}
		data_[i].conditionTime = int(v2Info_[i][V2ItemNames::kConditionTime]) * 60;
		bestData_[i].conditionTime = int(v2Info_[i][V2ItemNames::kConditionTime]) * 60;
	}
}

void StageData::SetData(int time, int itemCount,int maxItemCount ,bool flag, int currentStage) {
	SetClearTime(time, currentStage);
	if (data_[currentStage].clearTime <= bestData_[currentStage].clearTime) {
		bestData_[currentStage].clearTime = data_[currentStage].clearTime;
	}
	SetClearItemCount(itemCount, currentStage);
	if (data_[currentStage].clearItemCount >= bestData_[currentStage].clearItemCount) {
		bestData_[currentStage].clearItemCount = data_[currentStage].clearItemCount;
	}
	SetConditionItemCount(maxItemCount, currentStage);
	bestData_[currentStage].conditionItemCount = data_[currentStage].conditionItemCount;
	SetClearFlag(flag, currentStage);
	if (data_[currentStage].isClear==true &&
		bestData_[currentStage].isClear == false) {
		bestData_[currentStage].isClear = true;
	}
}
