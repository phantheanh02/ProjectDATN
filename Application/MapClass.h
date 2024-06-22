#pragma once
#include <unordered_map>
#include <memory>
#include "../Utilities/Math.h"
#include "Globals.h"
#include "Enemies.h"
#include "Item.h"

class Item;

enum PlanetType
{
	MT_BAREN = 0,
	MT_LAVA,
	MT_ICE,
	//MT_BLACK_HOLE,
	MT_TERRAN,
	MAP_SOLO
};
struct TileData
{
	int x, y;
	int gid; // Tile ID
};

struct ItemData
{
	ItemType type;
	GLfloat posX, posY;

	ItemData(ItemType _type, GLfloat _posX, GLfloat _posY) :
		type(_type), posX(_posX), posY(_posY) {};
};

struct EnemiesSpawn
{
	std::unordered_map<EnemyType, GLfloat>	enemiesTypeRatio;
	std::vector<Vector2>					spawnPosition;
};

struct MapData
{
	int width, height, minTileSize, maxTileSize;
	std::vector<TileData>	tiles;
	std::vector<ItemData>	items;
	EnemiesSpawn			enemiesSpawn;

	MapData() {};
};


class MapClass
{
public:
	MapClass(PlanetType type);
	~MapClass();

	void Init(GLint idTexture, GLint minTileSize, GLint maxTileSize, Vector2 sizeByTile);
	void Update(GLfloat deltaTime, b2Vec2 positionPlayer);
	void Draw();
	void OnMouseScroll();
	inline std::shared_ptr<MapData>	GetData() { return m_data; };
	inline PlanetType				GetType() { return m_type; };
	inline GLint					GetMinTileSize() { return m_data->minTileSize; };
	inline GLint					GetMaxTileSize() { return m_data->maxTileSize; };
	inline Vector2					GetSizeByTile() { return Vector2(m_data->width, m_data->height); };

private:
	std::shared_ptr<MapData>		m_data;

	PlanetType	m_type;
	GLint		m_idTexture;

	// Enemeis list
	std::shared_ptr<Sprite2D>				m_background;
};