// TrainingFramework.cpp : Defines the entry point for the console application.
//
#define SDL_MAIN_HANDLED
#include "stdafx.h"
#include <memory>
#include <conio.h>
#include <vector>
#include <iostream>
#include "../Utilities/utilities.h" // if you use STL, please include this line AFTER all other include
#include "Globals.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"
#include "SocketManager.h"

bool running;

int Init ( ESContext *esContext )
{
	glClearColor ( 1.0f, 1.0f, 1.0f, 1.0f );
	// Set up OpenGL blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// init
	ResourcesManager::Construct();
	ResourcesManager::GetInstance()->Init();
	ResourcesManager::GetInstance()->LoadResources("RM.data");
	SceneManager::Construct();
	SceneManager::GetInstance()->Init();
	SceneManager::GetInstance()->LoadElements("SM.data");
	GameStateMachine::Construct();
	GameStateMachine::GetInstance()->Init();
	GameStateMachine::GetInstance()->PushState(StateType::STATE_INTRO);

	//Net
	SocketManager::Construct();
	return 0;

}

void Draw ( ESContext *esContext )
{
	glClear(GL_COLOR_BUFFER_BIT);

	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->Draw();
	}
	eglSwapBuffers ( esContext->eglDisplay, esContext->eglSurface );
}

void Update ( ESContext *esContext, float deltaTime )
{
	esContext->running = GameStateMachine::GetInstance()->IsRunning();
	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->Update(deltaTime);
	}
	GameStateMachine::GetInstance()->Update();
}

void Key ( ESContext *esContext, unsigned char key, bool bIsPressed)
{
	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->OnKey(key, bIsPressed);
	}
}

void OnMouseClick(ESContext* esContext, GLint x, GLint y, unsigned char key, bool pressed)
{
	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->OnMouseClick(x, y, key, pressed);
	}
}

void OnMouseMove(ESContext* esContext, GLint x, GLint y)
{
	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->OnMouseMove(x, y);
	}
}

void OnMouseScroll(ESContext* esContext, GLint x, GLint y, short delta)
{
	if (esContext->running)
	{
		GameStateMachine::GetInstance()->GetCurrentState()->OnMouseScroll(x, y, delta);
	}
}

void CleanUp()
{
	SceneManager::GetInstance()->CleanUp();
	SceneManager::GetInstance()->DestroyInstance();
	ResourcesManager::GetInstance()->CleanUp();
	ResourcesManager::DestroyInstance();
	GameStateMachine::GetInstance()->CleanUp();
	GameStateMachine::DestroyInstance();
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Define the gravity vector.

	ESContext esContext;

    esInitContext ( &esContext );

	esCreateWindow ( &esContext, "Shadow Strike: Mercenary Mayhem", Globals::screenWidth, Globals::screenHeight, ES_WINDOW_RGB | ES_WINDOW_DEPTH);

	if (Init(&esContext) != 0)
	{
		return 0;
	}
	if (!ResourcesManager::GetInstance()->m_init)
	{
		std::cerr << "Resources initialization failed\n";
		return 0;
	}
	if (!SceneManager::GetInstance()->m_init)
	{
		std::cerr << "Scene initialization failed\n";
		return 0;
	}
	esRegisterDrawFunc ( &esContext, Draw );
	esRegisterUpdateFunc ( &esContext, Update );
	esRegisterKeyFunc ( &esContext, Key);
	esRegisterMouseButtonFunc(&esContext, OnMouseClick);
	esRegisterMouseScrollFunc(&esContext, OnMouseScroll);
	esRegisterMouseMoveFunc(&esContext, OnMouseMove);

	esMainLoop ( &esContext );

	//releasing OpenGL resources
	CleanUp();

	return 0;
}

