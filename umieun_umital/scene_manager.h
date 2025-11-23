#pragma once
#include "headers.h"

class Scene {
public:
    virtual ~Scene() {}
    virtual void Init() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void OnPause() {}
    virtual void OnResume() {}
    virtual void Draw() = 0;
    virtual void Finish() = 0;
    virtual void Reshape(int w, int h) {}
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
};