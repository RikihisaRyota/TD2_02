#include "StageData.h"

#include "GlobalVariables/GlobalVariables.h"

StageData::Data StageData::data_[kStageCount];

std::string StageData::stageNames_[Stage::kStageCount] = {
		"ステージ1",
		"ステージ2",
		"ステージ3",
		"ステージ4",
		"ステージ5",
		"ステージ6",
		"ステージ7",
		"ステージ8",
};
std::string StageData::v2ItemNames_[V2ItemNames::kV2ItemCount] = {
		"クリア条件タイム/クリア条件アイテム数",
};
Vector2 StageData::v2Info_[Stage::kStageCount][V2ItemNames::kV2ItemCount];
std::string StageData::groupName_ = "stageData";


StageData::StageData() {
	SetGlobalVariable();
}

void StageData::Initialize() {
	SetGlobalVariable();
}

void StageData::Update() {
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG
}

void StageData::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < Stage::kStageCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, stageNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void StageData::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < Stage::kStageCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector2Value(groupName_, stageNames_[i] + v2ItemNames_[j]);
		}
		v2Info_[i][V2ItemNames::kConditionTime].x = static_cast<float>(static_cast<int>(v2Info_[i][V2ItemNames::kConditionTime].x));
		v2Info_[i][V2ItemNames::kConditionTime].y = static_cast<float>(static_cast<int>(v2Info_[i][V2ItemNames::kConditionTime].y));
		data_[i].conditionTime = int(v2Info_[i][V2ItemNames::kConditionTime].x) * 60;
		data_[i].conditionItemCount = int(v2Info_[i][V2ItemNames::kConditionTime].y);
	}
}

void StageData::SetData(int time, int itemCount, bool flag, int currentStage) {
	SetClearTime(time, currentStage);
	SetClearItemCount(itemCount, currentStage);
	SetClearFlag(flag, currentStage);
}
