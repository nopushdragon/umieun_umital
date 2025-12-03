#pragma once
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include "scene_manager.h"
#include "GameTimer.h" // 기존 타이머 헤더 사용
#include "headers.h"

#ifdef _WIN32
#include <Windows.h>
#endif

// 전역 포인터 (GLUT 콜백이 정적 함수여야 해서 필요함)
class GameFramework;
extern GameFramework* g_Framework;

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
        glutCreateWindow("ㅇㅅㅇ");

        // 윈도우 크기 조절 비활성화 (Windows 전용)
#ifdef _WIN32
        HWND hwnd = FindWindow(NULL, L"ㅇㅅㅇ");
        if (hwnd) {
            LONG style = GetWindowLong(hwnd, GWL_STYLE);
            style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX); // 크기 조절 및 최대화 버튼 비활성화
            SetWindowLong(hwnd, GWL_STYLE, style);
            SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
#endif

        glewExperimental = GL_TRUE;
        if (glewInit() != GLEW_OK) {
            std::cerr << "GLEW Init Failed!" << std::endl;
            exit(-1);
        }

        // 콜백 함수 등록 (정적 함수 사용)
        glutDisplayFunc(RenderWrapper);
        glutReshapeFunc(ReshapeWrapper);
        glutTimerFunc(16, TimerWrapper, 0); // 약 60fps
        glutMouseFunc(MouseWrapper); //마우스함수이에오
        glutMotionFunc(MotionWrapper); //이건 마우스 움직임
        glutKeyboardFunc(KeyboardWrapper); //이건 키보드를 눌렀을 때
		glutKeyboardUpFunc(KeyupboardWrapper); //이건 키보드 뗐을 때
		glutMouseWheelFunc(MouseWheelWrapper); //이건 마우스 휠
		glutSpecialFunc(SpecialKeyboardWrapper); //특수키 함수
		glutPassiveMotionFunc(PassiveMotionWrapper); //이건 마우스가 움직일 때
		glutSpecialUpFunc(SpecialUpKeyboardWrapper); //특수키 뗐을 때

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
    static void MouseWrapper(int button, int state, int x, int y)
    {
        g_Framework->sceneManager->Mouse(button,state,x,y);
    }
	static void MotionWrapper(int x, int y)
	{
		g_Framework->sceneManager->Motion(x, y);
	}
    static void KeyboardWrapper(unsigned char key, int x, int y)
    {
        g_Framework->sceneManager->Keyboard(key,x, y);
    }
	static void KeyupboardWrapper(unsigned char key, int x, int y)
    {
        g_Framework->sceneManager->Keyupboard(key, x, y);
    }
    static void MouseWheelWrapper(int wheel, int direction, int x, int y)
    {
        g_Framework->sceneManager->MouseWheel(wheel, direction, x, y);
    }
    static void SpecialKeyboardWrapper(int key, int x, int y)
	{
		g_Framework->sceneManager->SpecialKeyboard(key, x, y);
	}
    static void PassiveMotionWrapper(int x, int y)
	{
		g_Framework->sceneManager->PassiveMotion(x, y);
	}
	static void SpecialUpKeyboardWrapper(int key, int x, int y)
	{
		g_Framework->sceneManager->SpecialUpKeyboard(key, x, y);
	}

    static void TimerWrapper(int value) {
        g_Framework->gameTimer->Update();
        float dt = g_Framework->gameTimer->elapsedTime; 

        g_Framework->sceneManager->Update(dt); 

        glutPostRedisplay();
        glutTimerFunc(16, TimerWrapper, 0);
    }
};