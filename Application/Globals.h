#pragma once
#include <string>
#include "GSChooseCharacter.h"

class Globals final
{
public:
	static const int screenWidth = 960;
	static const int screenHeight = 720;
	inline static const std::string modelPath = "../Data/Models/";
	inline static const std::string shaderPath = "../Data/Shaders/";
	inline static const std::string texturePath = "../Data/Textures/";
	inline static const std::string soundPath = "../Data/Sounds/";
	inline static const std::string scenePath = "../Data/Scenes/";
	inline static const std::string fontPath = "../Data/Fonts/"; 
	
};

// flags
#define CHEAT 0
#define DEBUG 1

// func
#define LOG(str) if (DEBUG) std::cout << str << "\n";

// key define
#define KEY_LEFT		VK_LEFT
#define KEY_RIGHT		VK_RIGHT
#define KEY_UP			VK_UP
#define KEY_DOWN		VK_DOWN
#define KEY_SPACE		VK_SPACE
#define KEY_LCTRL		VK_LCONTROL
#define KEY_ESC			VK_ESCAPE
#define KEY_W			'W'
#define KEY_A			'A'
#define KEY_S			'S'
#define KEY_D			'D'
#define KEY_E			'E'
#define KEY_J			'J'

// Color
#define	YELLOW			255, 255, 0
#define RED				255, 0, 0
#define BLUE			0, 0, 255
#define GREEN			0, 255, 0
#define BLACK			0, 0, 0
#define WHITE			255, 255, 255

// Time
#define	TIME_END_3S					10
#define TIME_END_2S					20
#define COOLDOWN_ENEMY_BULLET		1.2f
#define TIME_FINDING				3.0f
#define JUMP_COOLDOWN				0.3
#define BULLET_COOLDOWN				0.3f

// Local wifi
#define MAX_CLIENTS 1

// other
#define DETECTION_DISTANCE			8.0f
#define DETECTION_DISTANCE_SNIPER	8.0f
#define VELOCITY_ITERATION			8
#define POSITION_ITERATION			3
#define MOVEMENT_SPEED				10.0f
#define LOADING_BULLET_TIME			0.9f
#define	SCALE_SIZE					2.8f
#define	MAX_DISTANCE				15