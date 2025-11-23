#include "scene_manager.h"

void SceneManager::ChangeScene(Scene* newScene) {
	while (!sceneStack.empty()) {
		sceneStack.back()->Exit();
		delete sceneStack.back();
		sceneStack.pop_back();
	}
	newScene->Enter();
	sceneStack.push_back(newScene);
}
void SceneManager::Push_Mode(Scene* newScene) {
	if (!sceneStack.empty()) {
		sceneStack.back()->OnPause();
	}
	newScene->Enter();
	sceneStack.push_back(newScene);
}
void SceneManager::Pop_Mode() {
	if (sceneStack.empty()) return;
	Scene* cur = sceneStack.back();
	cur->Exit();
	delete cur;
	sceneStack.pop_back();
	if (!sceneStack.empty()) {
		sceneStack.back()->OnResume();
	}
}
SceneManager::~SceneManager() {
	while (!sceneStack.empty()) {
		Pop_Mode();
	}
}

void SceneManager::Update(float deltatime) {
	if (!sceneStack.empty()) {
		sceneStack.back()->Update(deltatime);
	}
}
void SceneManager::Draw() {
	for (Scene* s : sceneStack) {
		s->Draw();
	}
}