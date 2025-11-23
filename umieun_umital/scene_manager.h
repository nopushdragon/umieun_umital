#pragma once
#include "headers.h"

class Scene {
public:
    virtual ~Scene();
    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void OnPause();
    virtual void OnResume();
    virtual void Draw() = 0;
    virtual void Finish() = 0;
    virtual void Reshape(int w, int h);

    virtual void Mouse(int button, int state, int x, int y);
    virtual void MouseWheel(int wheel, int direction, int x, int y);
	
    virtual void Motion(int x, int y);
    virtual void PassiveMotion(int x, int y);
	
    virtual void Keyboard(unsigned char key, int x, int y);
    virtual void Keyupboard(unsigned char key, int x, int y);
	
    virtual void SpecialKeyboard(int key, int x, int y);
    virtual void SpecialUpKeyboard(int key, int x, int y);


};

class SceneManager {
private:
     std::vector<Scene*> sceneStack;

public:

    ~SceneManager();

    void Change_Mode(Scene* newScene);


    void Push_Mode(Scene* newScene);

  
    void Pop_Mode();

    void Update(float deltatime);

    void Draw();

    void Reshape(int w, int h);

    void Mouse(int button, int state, int x, int y);
	void MouseWheel(int wheel, int direction, int x, int y);
	void Motion(int x, int y);
	void PassiveMotion(int x, int y);
	void Keyboard(unsigned char key, int x, int y);
	void Keyupboard(unsigned char key, int x, int y);
	void SpecialKeyboard(int key, int x, int y);
	void SpecialUpKeyboard(int key, int x, int y);

};