#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "scene_manager.h"
#include "GameTimer.h" // 기존 타이머 헤더 사용
#include "headers.h"

// 전역 포인터 (GLUT 콜백이 정적 함수여야 해서 필요함)
class GameFramework;
GameFramework* g_Framework = nullptr;

class GameFramework {
public:
    SceneManager* sceneManager;
    GameTimer* gameTimer;


    GameFramework() {
        sceneManager = new SceneManager();
        gameTimer = new GameTimer();
        g_Framework = this; // 전역 포인터 연결
    }

    ~GameFramework() {
        delete sceneManager;
        delete gameTimer;
    }

    void Init(int argc, char** argv) {
        glutInit(&argc, argv);
        glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
        glutInitWindowSize(winWidth, winHeight);
        glutCreateWindow("3D Maze Framework");

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "GLEW Init Failed!" << std::endl;
            exit(-1);
        }

        // 콜백 함수 등록 (정적 함수 사용)
        glutDisplayFunc(RenderWrapper);
        glutReshapeFunc(ReshapeWrapper);
        glutTimerFunc(16, TimerWrapper, 0); // 약 60fps
    }

    void Run(Scene* startScene) {
        sceneManager->Change_Mode(startScene);
        glutMainLoop();
    }

    // --- GLUT Static Callbacks ---
    static void RenderWrapper() {
        g_Framework->sceneManager->Draw();
        glutSwapBuffers();
    }

    static void ReshapeWrapper(int w, int h) {
        winWidth = w;
        winHeight = h;
        glViewport(0, 0, w, h);
        g_Framework->sceneManager->Reshape(w, h);
    }

    static void TimerWrapper(int value) {
        g_Framework->gameTimer->Update();
        float dt = g_Framework->gameTimer->elapsedTime; 

        g_Framework->sceneManager->Update(dt); 

        glutPostRedisplay();
        glutTimerFunc(16, TimerWrapper, 0);
    }
};