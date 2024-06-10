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

// other
#define HP_MAX						50
#define ARMOR_MAX					100
#define NUMBER_BULLET_MAX			100
#define DETECTION_DISTANCE			8.0f
#define DETECTION_DISTANCE_SNIPER	8.0f
#define HEIGHT_BULLET				10.0f
#define BULLET_SIZE					0.1f
#define VELOCITY_ITERATION			8
#define POSITION_ITERATION			3
#define MOVEMENT_SPEED				7.0f
#define BULLET_COOLDOWN				0.3f

#define	SCALE_SIZE					3.0f
#define	HEIGHT_BOX					0.666667f
#define HEIGHT_IMG					(30 / 48) // để chuẩn hóa, 1 ô = 48 pixel, 30 là chiều cao chung

#define	MAX_DISTANCE				15
#define COOLDOWN_ENEMY_BULLET		1.2f
#define TIME_FINDING				3.0f