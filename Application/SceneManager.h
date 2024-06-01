#pragma once
#include <unordered_map>
#include "Singleton.h"
#include "Camera.h"
#include "Sprite2D.h"
#include "ConfigClass.h"
#include "MapClass.h"
#include "Enemies.h"

enum ElementType
{
	ET_CAMERA,
	ET_OBJECT,
	ET_ENEMY,
	ET_BULLET,
	ET_MAP,
	ET_CHARACTER,
	MT_PLANES,
	MT_ENEMIES,
	MT_ITEMS,
	ET_INVALID = -1
};

enum CameraType
{
	STATIC_CAMERA,
	DYNAMIC_CAMERA,
};

struct CharacterStats
{
	CharacterType type;
	GLint hp, spd, atk;
	CharacterStats() : hp(0), spd(0), atk(0) {};
	CharacterStats(GLint _hp, GLint _spd, GLint _atk)
		: hp(_hp), spd(_spd), atk(_atk) {};

};

class SceneManager final : public SingletonDclp<SceneManager>
{
public:
	// APIs
	void Init();
	void CleanUp();
	void LoadElements(const std::string& filename);

	std::shared_ptr<Camera>		GetCamera(CameraType camera);
	std::shared_ptr<Sprite2D>	GetObject(GLint object_id);
	std::shared_ptr<MapClass>	GetMap(PlanetType type);
	BoxBullet					GetBoxBullet(GLint bullet_id);
	std::shared_ptr<Enemies>	GetEnemy(EnemyType type);
	CharacterStats				GetCharacterStats(CharacterType type);

	bool m_init;
	GLint m_soundVolume;
	GLint m_lastNonZeroSoundVolume;

private:
	std::unordered_map<CameraType, std::shared_ptr<Camera>>	m_cameraList;
	std::unordered_map<GLint, std::shared_ptr<Sprite2D>>	m_objectList;
	std::unordered_map<GLint, BoxBullet>					m_bulletList;
	std::vector<std::shared_ptr<Enemies>>					m_enemiesList;
	std::vector<std::shared_ptr<MapClass>>					m_mapList;
	std::vector<CharacterStats>								m_characterList;

	// Utilities
	void LoadObject(std::ifstream& file);
	void LoadCamera(std::ifstream& file);
	void LoadEnemies(std::ifstream& file);
	void LoadBullet(std::ifstream& file);
	void LoadCharacter(std::ifstream& file);

	// Load map
	void					LoadMap(std::ifstream& file);
	std::shared_ptr<MapClass>	LoadElementsMap(std::ifstream& file, std::shared_ptr<MapClass> map);
	void					LoadPlanesMap(std::ifstream& file, std::shared_ptr<MapClass> map);
	void					LoadSpawnEnemies(std::ifstream& file, std::shared_ptr<MapClass> map);
	void					LoadItemsMap(std::ifstream& file, std::shared_ptr<MapClass> map);


};
