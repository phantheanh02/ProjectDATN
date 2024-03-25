#pragma once
#include <unordered_map>
#include "Singleton.h"
#include "Camera.h"
#include "Sprite2D.h"
#include "Enemies.h"
#include "ConfigClass.h"

enum ElementType
{
	ET_CAMERA,
	ET_OBJECT,
	ET_ENEMY,
	ET_BULLET,
	ET_INVALID = -1
};


class SceneManager final : public SingletonDclp<SceneManager>
{
public:
	// APIs
	void Init();
	void CleanUp();
	void LoadElements(const std::string& filename);
	std::shared_ptr<Camera> GetCamera(GLint camera_id);
	std::shared_ptr<Sprite2D> GetObject(GLint object_id);
	BoxEnemy GetBoxEnemy(GLint enemy_id);
	BoxBullet GetBoxBullet(GLint bullet_id);

	bool m_init;
	GLint m_soundVolume;
	GLint m_lastNonZeroSoundVolume;

private:
	std::unordered_map<GLint, std::shared_ptr<Camera>> m_cameraList;
	std::unordered_map<GLint, std::shared_ptr<Sprite2D>> m_objectList;
	std::unordered_map<GLint, BoxEnemy> m_enemiesList;
	std::unordered_map<GLint, BoxBullet> m_bulletList;

	// Utilities
	void LoadObject(std::ifstream& file);
	void LoadCamera(std::ifstream& file);
	void LoadEnemies(std::ifstream& file);
	void LoadBullet(std::ifstream& file);
};