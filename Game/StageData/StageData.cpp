#include "StageData.h"

StageData::Data StageData::data_[kStageCount];

StageData::StageData() {}

void StageData::SetData(int time, int itemCount, bool flag, int currentStage) {
	SetClearTime(time,currentStage);
	SetClearItemCount(itemCount, currentStage);
	SetClearFlag(flag, currentStage);
}
