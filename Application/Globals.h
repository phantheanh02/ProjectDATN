#pragma once
#include <string>

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

constexpr auto TILESIZEBYPIXEL = 44;

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
