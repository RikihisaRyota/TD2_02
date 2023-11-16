#include "ModelManager.h"

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;
	return &instance;
}

void ModelManager::LoadBlockModel(std::initializer_list<std::string> modelNames, bool isLighting) {
	for (auto& modelName : modelNames) {
		blockModels_.emplace_back(Model::Create(modelName, isLighting));
	}
}

void ModelManager::LoadModel(std::initializer_list<std::string> modeNames, bool isLighting) {
	for (auto& modelName: modeNames) {
		models_.insert(std::make_pair(modelName, Model::Create(modelName, isLighting)));
	}
}

Model* ModelManager::GetModel(std::string modeName) {
	return models_.at(modeName);
}

void ModelManager::Shutdown() {
	// マップ内の各 Model ポインタを解放
	for (auto it = models_.begin(); it != models_.end(); ++it) {
		delete it->second; // Model オブジェクトの解放
	}

	// マップをクリア
	models_.clear();
	for (auto& blockModel : blockModels_) {
		delete blockModel;
	}
	blockModels_.clear();
}
