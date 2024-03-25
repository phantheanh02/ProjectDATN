#pragma once
#include <iostream>
#include <unordered_map>
#include <memory>
#include <string>
#include "Singleton.h"
#include "Model.h"
#include "Shaders.h"
#include "Texture.h"
#include "Camera.h"
#include "Sound.h"
#include "SDL.h"
#include "SDL_mixer.h"

class ResourcesManager : public SingletonDclp<ResourcesManager>
{
public:
	void Init();
	std::shared_ptr<Model> getModel(const std::string& filename);
	std::shared_ptr<Shaders> getShader(const std::string& filename);
	std::shared_ptr<Texture> getTexture(const std::string& filename);

	std::shared_ptr<Sound> getSound(const std::string& filename);
	void SetMasterVolume(int volume);
	void PauseAllSounds();
	void StopAllSounds();
	void ResumeAllSounds();
	void CleanUp();
	bool m_init;
private:
	std::unordered_map<std::string, std::shared_ptr<Model>> m_modelList;
	std::unordered_map<std::string, std::shared_ptr<Shaders>> m_shaderList;
	std::unordered_map<std::string, std::shared_ptr<Texture>> m_textureList;
	std::unordered_map<std::string, std::shared_ptr<Sound>> m_soundList;
};