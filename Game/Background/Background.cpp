#include "Background.h"

#include "ImGuiManager.h"

Background::Background() {
	model_.reset(Model::Create("background"));
	worldTransform_.Initialize();
	// デバック用
	Initialize();
}

void Background::Initialize() {
	worldTransform_.translate_ = {98.0f,74.0f,160.0f};
	worldTransform_.UpdateMatrix();
}

void Background::Update() {
	ImGui::Begin("Debug");
	if (ImGui::TreeNode("Background")) {
		ImGui::DragFloat3("position", &worldTransform_.translate_.x,1.0f);
		ImGui::TreePop();
	}
	ImGui::End();
	worldTransform_.UpdateMatrix();
}

void Background::Draw(const ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection);
}
