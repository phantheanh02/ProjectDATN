#include "stdafx.h"
#include <iostream>
#include <fstream>
#include "SceneManager.h"
#include "ResourcesManager.h"
#include "Globals.h"
#include "nlohmann/json.hpp"
#include "Player.h"
#include "Bullet.h"

using json = nlohmann::json;

int ParseElementType(const std::string& type);

void SceneManager::Init()
{
	m_init = true;
	// dynamic camera for objects
	std::shared_ptr<Camera> dynamicCamera = std::make_shared<Camera>(0);

	// static camera for UI
	std::shared_ptr<Camera> staticCamera = std::make_shared<Camera>(1);

	m_cameraList.insert(std::make_pair(CameraType::DYNAMIC_CAMERA, dynamicCamera));
	m_cameraList.insert(std::make_pair(CameraType::STATIC_CAMERA, staticCamera));

	m_soundVolume = 100;
	m_lastNonZeroSoundVolume = 100;
}

void SceneManager::CleanUp()
{
	for (auto& it : m_cameraList)
	{
		it.second.reset();
	}
	for (auto& it : m_objectList)
	{
		it.second.reset();
	}
	m_cameraList.clear();
	m_objectList.clear();
}

void SceneManager::LoadElements(const std::string& filename)
{
	std::string path = Globals::scenePath + filename;
	std::ifstream fscene(path);
	if (!fscene.is_open())
	{
		std::cerr << "Error opening file " << path << "\n";
		m_init = false;
		return;
	}
	std::string typeStr;
	GLint type;
	bool valid = true;
	do
	{
		if (!(fscene >> typeStr))
		{
			break;
		}
		type = ParseElementType(typeStr);
		switch (type)
		{
		case ET_CAMERA:
			LoadCamera(fscene);
			break;
		case ET_OBJECT:
			LoadObject(fscene);
			break;
		case ET_ENEMY:
			LoadEnemies(fscene);
			break;
		case ET_BULLET:
			LoadBullet(fscene);
			break;
		case ET_MAP:
			LoadMap(fscene);
			break;
		case ET_CHARACTER:
			LoadCharacter(fscene);
			break;
		default:
			std::cerr << "Scene file format error, abort loading elements";
			valid = false;
			m_init = false;
			break;
		}
	} while (valid);
	fscene.close();
}


std::shared_ptr<Camera> SceneManager::GetCamera(CameraType camera)
{
	switch (camera)
	{
	case STATIC_CAMERA:
		return m_cameraList[STATIC_CAMERA];
	case DYNAMIC_CAMERA:
		return m_cameraList[DYNAMIC_CAMERA];
	default:
		return nullptr;
	}
	return nullptr;
}

std::shared_ptr<Sprite2D> SceneManager::GetObject(GLint object_id)
{
	auto it = m_objectList.find(object_id);
	if (it != m_objectList.end())
	{
		return it->second;
	}
	std::cerr << "ERR: Object with id " << object_id << " not found!\n";
	return nullptr;
}

BoxBullet SceneManager::GetBoxBullet(GLint bullet_id)
{
	for (auto it : m_bulletList)
	{
		if (it.first == bullet_id)
		{
			return it.second;
		}
	}
	std::cerr << "ERR: Enemy with id " << bullet_id << " not found!\n";
	return BoxBullet();
}

std::shared_ptr<MapClass> SceneManager::GetMap(PlanetType type)
{
	for (auto it : m_mapList)
	{
		if (it->GetType() == type)
		{
			return it;
		}
	}
	std::cerr << "ERR: Map with type " << type << " not found!\n";
	return nullptr;
}

std::shared_ptr<Enemies> SceneManager::GetEnemy(EnemyType type)
{
	for (auto enemy : m_enemiesList)
	{
		if (enemy->GetType() == type)
		{
			return enemy->Clone();
		}
	}
	return nullptr;
}

CharacterStats SceneManager::GetCharacterStats(CharacterType type)
{
	for (auto character : m_characterList)
	{
		if (character.type == type)
		{
			return character;
		}
	}
	return CharacterStats();
}

void SceneManager::LoadObject(std::ifstream& file)
{
	Vector3 pos, rt, sc;
	GLint id, modelId, textureId, shaderId;
	std::string skipStr;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	while (skipStr != "$")
	{
		file >> skipStr;
		//file >> skipStr >> id >> skipStr >> modelId >> skipStr >> textureId >> skipStr >> shaderId;
		//file >> skipStr >> pos.x >> pos.y >> pos.z;
		//file >> skipStr >> rt.x >> rt.y >> rt.z;
		//file >> skipStr >> sc.x >> sc.y >> sc.z;
		//auto model = ResourcesManager::GetInstance()->GetModel(modelId);
		//auto shader = ResourcesManager::GetInstance()->GetShader(shaderId);
		//auto texture = ResourcesManager::GetInstance()->GetTexture(textureId);
		//auto obj = std::make_shared<Sprite2D>(id, model, shader, texture);
		//obj->SetPos(pos);
		//obj->SetScale(sc);
		//obj->SetRotation(rt);
		//m_objectList.insert(std::make_pair(id, obj));
	}
}

void SceneManager::LoadCamera(std::ifstream& file)
{
	std::string skipStr;
	Vector3 pos;
	GLint id;
	GLfloat nr, fr, fov, mspeed, rspeed;
	file >> skipStr;
	while (skipStr != "$")
	{
		file >> skipStr;
		//file >> skipStr >> id;
		//file >> skipStr >> pos.x >> pos.y >> pos.z;
		//file >> skipStr >> nr >> skipStr >> fr;
		//file >> skipStr >> fov;
		//file >> skipStr >> mspeed;
		//file >> skipStr >> rspeed;
		//auto camera = std::make_shared<Camera>(id, pos, nr, fr, fov, rspeed, mspeed);
		//m_cameraList.insert(std::make_pair(id, camera));
	}
}

void SceneManager::LoadEnemies(std::ifstream& file)
{
	std::string skipStr;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	file >> skipStr;
	GLint id;
	Vector2 imgSize, boxSize;
	while (skipStr != "$")
	{
		id = std::stoi(skipStr);
		file >> imgSize.x >> imgSize.y >> boxSize.x >> boxSize.y >> skipStr;
		auto enemy = std::make_shared<Enemies>((EnemyType)id, imgSize, boxSize);
		m_enemiesList.push_back(enemy);
	}
}

void SceneManager::LoadBullet(std::ifstream& file)
{
	std::string skipStr;
	BoxBullet box;
	file >> skipStr >> skipStr >> skipStr;
	file >> skipStr;
	while (skipStr != "$")
	{
		BoxBullet box;
		box.id = std::stoi(skipStr);
		file >> box.imgSize.x >> box.imgSize.y >> skipStr;
		m_bulletList.insert(std::make_pair(box.id, box));
	}
}

void SceneManager::LoadCharacter(std::ifstream& file)
{
	std::string skipStr;
	BoxBullet box;
	file >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	file >> skipStr;
	while (skipStr != "$")
	{
		CharacterStats character;
		character.type = (CharacterType)std::stoi(skipStr);
		file >> character.hp >> character.spd >> character.atk >> character.numberBullet >> skipStr;
		m_characterList.push_back(character);
	}
}

void SceneManager::LoadMap(std::ifstream& file)
{
	std::string skipStr, filemap;
	int id = 0;
	file >> skipStr;
	while (skipStr != "$")
	{
		id = std::stoi(skipStr);
		std::shared_ptr<MapClass> map = std::make_shared<MapClass>((PlanetType)id);
		file >> filemap >> skipStr;
		std::string path = Globals::scenePath + filemap;
		std::ifstream fFile(path);
		if (!fFile.is_open())
		{
			std::cerr << "Error opening file " << path << "\n";
			return;
		}
		LoadElementsMap(fFile, map);
		m_mapList.push_back(map);
	}
}

std::shared_ptr<MapClass> SceneManager::LoadElementsMap(std::ifstream& file, std::shared_ptr<MapClass> map)
{
	GLint idTexture, minTileSize, maxTileSize;
	Vector2 sizeByTile;

	std::string skipStr;
	file >> skipStr >> idTexture >> skipStr >> sizeByTile.x >> sizeByTile.y >> minTileSize >> maxTileSize;
	map->Init(idTexture, minTileSize, maxTileSize, sizeByTile);
	std::string typeStr;
	GLint type;
	do
	{
		if (!(file >> typeStr))
		{
			break;
		}
		type = ParseElementType(typeStr);
		switch (type)
		{
		case MT_PLANES:
			LoadPlanesMap(file, map);
			break;
		case MT_ENEMIES:
			LoadSpawnEnemies(file, map);
			break;
		case MT_ITEMS:
			LoadItemsMap(file, map);
			break;
		default:
			//std::cerr << "Map file format error, abort loading elements\n";
			file.close();
			return map;
		}
	} while (true);
	file.close();
	return map;
}

void SceneManager::LoadPlanesMap(std::ifstream& file, std::shared_ptr<MapClass> map)
{
	//std::string skipStr;
	//float x, y, z, w;
	//std::vector<Vector4> planeList;
	//file >> skipStr;
	//while (skipStr != "$")
	//{
	//	x = std::stoi(skipStr);
	//	file >> y >> z >> w >> skipStr;
	//	planeList.push_back(Vector4(x, y, z, w));
	//}
	//map->SetPlaneList(planeList);
	std::string filemap;
	file >> filemap;
	
	std::string path = Globals::scenePath + filemap;
	std::ifstream fFile(path);
	if (!fFile.is_open())
	{
		std::cerr << "Error opening file " << path << "\n";
		return;
	}

	json data;
	fFile >> data;

	MapData mapData;
	mapData.width = data["width"];
	mapData.height = data["height"];
	mapData.tileWidth = data["tilewidth"];
	mapData.tileHeight = data["tileheight"];

	for (const auto& layer : data["layers"])
	{
		for (const auto& chunk : layer["chunks"])
		{

			int chunkX = chunk["x"];
			int chunkY = chunk["y"];
			int chunkWidth = chunk["width"];
			int chunkHeight = chunk["height"];
			const auto& data = chunk["data"];

			for (int i = 0; i < chunkHeight; ++i)
			{
				for (int j = 0; j < chunkWidth; ++j)
				{
					int gid = data[i * chunkWidth + j];
					if (gid != 0)
					{ // Non-empty tile
						TileData tile;
						tile.x = chunkX + j;
						tile.y = chunkY + i;
						tile.gid = gid;
						mapData.tiles.push_back(tile);
					}
				}
			}
		}
	}
	map->SetPlaneList(mapData);
	file >> filemap;
}

void SceneManager::LoadSpawnEnemies(std::ifstream& file, std::shared_ptr<MapClass> map)
{
	std::string skipStr;
	int type, x, y;
	GLfloat ratio;
	std::unordered_map<EnemyType, GLfloat>	enemiesTypeRatio;
	std::vector<Vector2>					spawnPosition;
	file >> skipStr >> skipStr >> skipStr;

	while (skipStr != "POSX")
	{
		type = std::stoi(skipStr);
		file >> ratio >> skipStr;
		enemiesTypeRatio.insert(std::make_pair((EnemyType)type, ratio));
	}

	file >> skipStr >> skipStr;
	while (skipStr != "$")
	{
		x = std::stoi(skipStr);
		file >> y >> skipStr;
		spawnPosition.push_back(Vector2(x, y));
	}
	
	map->SetEnemiesTypeRatio(enemiesTypeRatio);
	map->SetSpawnPosition(spawnPosition);
}

void SceneManager::LoadItemsMap(std::ifstream& file, std::shared_ptr<MapClass> map)
{
	std::string skipStr;
	int x, y, z;
	std::vector<Vector3> itemList;
	file >> skipStr >> skipStr >> skipStr >> skipStr;
	while (skipStr != "$")
	{
		x = std::stoi(skipStr);
		file >> y >> z >> skipStr;
		itemList.push_back(Vector3(x, y, z));
	}
	map->SetItemList(itemList);
}

int ParseElementType(const std::string& type)
{
	if (type == "#Objects")
		return ET_OBJECT;
	if (type == "#Cameras")
		return ET_CAMERA;
	if (type == "#Enemies")
		return ET_ENEMY;
	if (type == "#Bullets")
		return ET_BULLET;
	if (type == "#Maps")
		return ET_MAP;
	if (type == "#PlanesMap")
		return MT_PLANES;
	if (type == "#SpawnEnemies")
		return MT_ENEMIES;
	if (type == "#ItemsMap")
		return MT_ITEMS;
	if (type == "#CharacterStats")
		return ET_CHARACTER;
	return ET_INVALID;
}
