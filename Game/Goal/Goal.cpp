#include "Goal.h"

#include "GlobalVariables/GlobalVariables.h"
#include "SceneSystem/IScene/IScene.h"
#include "TextureManager.h"
Goal::Goal() {
	model_ = std::make_unique<PlaneRenderer>();
	model_.reset(PlaneRenderer::Create());
	texture_ = TextureManager::Load("Resources/Textures/uvChecker.png");

	worldTransform_.Initialize();
}

void Goal::Initialize() {
	SetGlobalVariable();
}

void Goal::Update() {
#ifdef _DEBUG
	ApplyGlobalVariable();
#endif // _DEBUG
}

void Goal::Draw(const ViewProjection& viewProjection) {

	model_->Draw(worldTransform_,viewProjection,texture_);
}

void Goal::SetGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	globalVariables->CreateGroup(groupName_);

	for (int i = 0; i < GoalNames::kGoalCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			globalVariables->AddItem(groupName_, spriteNames_[i] + v2ItemNames_[j], v2Info_[i][j]);
		}
	}

	ApplyGlobalVariable();
}

void Goal::ApplyGlobalVariable() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	for (int i = 0; i < GoalNames::kGoalCount; i++) {
		for (int j = 0; j < V2ItemNames::kV2ItemCount; j++) {
			v2Info_[i][j] = globalVariables->GetVector3Value(groupName_, spriteNames_[i] + v2ItemNames_[j]);
		}
	}
	worldTransform_.translate_ = v2Info_[IScene::stageNo_][V2ItemNames::kPos];
	worldTransform_.UpdateMatrix();
}
