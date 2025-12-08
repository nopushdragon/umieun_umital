#include "scene_manager.h"


Scene::~Scene() {

}

void Scene::OnPause() {

}

void Scene::OnResume() {

}

void Scene::Reshape(int w, int h) {

}

void Scene::Mouse(int button, int state, int x, int y) {

}

void Scene::MouseWheel(int wheel, int direction, int x, int y){

}

void Scene::Motion(int x, int y) {

}
void Scene::PassiveMotion(int x, int y) {

}
void Scene::Keyboard(unsigned char key, int x, int y) {

}
void Scene::Keyupboard(unsigned char key, int x, int y) {

}
void Scene::SpecialKeyboard(int key, int x, int y) {

}
void Scene::SpecialUpKeyboard(int key, int x, int y) {

}


void SceneManager::Change_Mode(Scene* newScene) {
	while (!sceneStack.empty()) {
		sceneStack.back()->Finish();
		delete sceneStack.back();
		sceneStack.pop_back();
	}
	newScene->Init();
	sceneStack.push_back(newScene);
}

void SceneManager::Push_Mode(Scene* newScene) {
	if (!sceneStack.empty()) {
		sceneStack.back()->OnPause();
	}
	newScene->Init();
	sceneStack.push_back(newScene);
}
void SceneManager::Pop_Mode() {
	if (sceneStack.empty()) return;
	Scene* cur = sceneStack.back();
	cur->Finish();
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

void SceneManager::Reshape(int w, int h) {
	if (!sceneStack.empty()) {
		for (Scene* s : sceneStack) {
			s->Reshape(w, h);
		}
	}
}

void SceneManager::Mouse(int button, int state, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->Mouse(button, state, x, y);
}
void SceneManager::MouseWheel(int wheel, int direction, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->MouseWheel(wheel, direction, x, y);
}
void SceneManager::Motion(int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->Motion(x, y);
}
void SceneManager::PassiveMotion(int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->PassiveMotion(x, y);
}
void SceneManager::Keyboard(unsigned char key, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->Keyboard(key, x, y);
}
void SceneManager::Keyupboard(unsigned char key, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->Keyupboard(key, x, y);
}
void SceneManager::SpecialKeyboard(int key, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->SpecialKeyboard(key, x, y);
}
void SceneManager::SpecialUpKeyboard(int key, int x, int y) {
	if (!sceneStack.empty()) sceneStack.back()->SpecialUpKeyboard(key, x, y);
}
