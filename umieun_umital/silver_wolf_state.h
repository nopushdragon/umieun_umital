#pragma once
#include "headers.h"

class silver_wolf;

class WolfState
{
public:
    virtual ~WolfState() {}

    virtual void Enter(silver_wolf* wolf) = 0;

    virtual void Update(silver_wolf* wolf, float dt) = 0;

    virtual void Draw(silver_wolf* wolf, GLuint shaderID, float time) = 0;

    virtual void Exit(silver_wolf* wolf) = 0;
};