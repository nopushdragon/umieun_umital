#pragma once
#include "headers.h"

class Scene {
public:
    virtual ~Scene() {}
    virtual void Enter() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void OnPause() {}
    virtual void OnResume() {}
    virtual void Draw() = 0;
    virtual void Exit() = 0;
};

class SceneManager {
private:
     std::vector<Scene*> sceneStack;

public:
    ~SceneManager();

    void ChangeScene(Scene* newScene);


    void Push_Mode(Scene* newScene);

  
    void Pop_Mode();

    void Update(float deltatime);

    void Draw();
};