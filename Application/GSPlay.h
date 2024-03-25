#pragma once
#include <memory>
#include "GameStateBase.h"
#include "Sprite2D.h"
#include "SpriteAnimation.h"
#include "Button.h"
#include "box2d.h"
#include "Text.h"
#include "Player.h"
#include "Enemies.h"
#include "b2Utilities.h"
#include "Bullet.h"
#include "Globals.h"
#include "Boss.h"
#include "Item.h"

constexpr auto VELOCITY_ITERATION = 8;
constexpr auto POSITION_ITERATION = 3;
constexpr auto MOVEMENT_SPEED = 7.0f;

class GSPlay final : public GameStateBase
{
public:
	using GameStateBase::GameStateBase;
	~GSPlay() override;

	void Init() override;
	void Update(float deltaTime) override;
	void Draw() override;
	void Pause() override;
	void Resume() override;
	void Exit() override;

	void HandleEvent();
	void OnKey(unsigned char key, bool pressed) override;
	void OnMouseClick(int x, int y, unsigned char key, bool pressed) override;
	void OnMouseMove(int x, int y) override;
	void OnMouseScroll(int x, int y, short delta) override;
private:
	GLint			m_numberEnemyKill;
	GLint			m_totalCoinCostBullet;
	GLint			m_totalCoinCostArmor;
	GLint			m_totalCoinCostHP;

	unsigned int	m_key;
	unsigned int	m_mouse;
	bool			m_isBossDie;
	bool			m_isPlayerDie;
	GLfloat			m_timeReplay;
	bool			m_isOnDynamicObs;
	bool			m_needUpdateDrawSize;
	GLfloat			m_timePlay;
	std::shared_ptr<Camera>				m_dynamicCamera;
	std::shared_ptr<Camera>				m_staticCamera;
	std::shared_ptr<Sprite2D>			m_map;
	std::shared_ptr<Player>				m_player;
	std::shared_ptr<Boss>				m_boss;
	std::vector<std::shared_ptr<Enemies>>	m_enemiesList;
	std::vector<std::shared_ptr<Bullet>>	m_bulletList;
	
	// box2d
	std::shared_ptr<b2World>			m_world;
	std::vector<b2Body*>				m_BodyList;
	ContactListener*					m_contactListener;
	b2Vec2								m_gravity;
	GLfloat								m_timeStep;
	GLfloat								m_lastJumpTime;
	GLfloat								m_lastShootTime;
	Vector4								m_cameraPositionBoudaries;
	Vector2								m_mapSizeByTile;
	GLint								m_minTileSize;
	GLint								m_maxTileSize;
	GLint								m_tileSizeByPixel = TILESIZEBYPIXEL;
	GLint								m_lastPlayerHorizontalDirection;

	// Obs dynamic
	std::vector<ObsDynamic>				m_obsDynamicList;
	std::vector<ObsDynamic>				m_barsList;

	// Item
	std::vector<std::shared_ptr<Item>>	m_itemList;

	// Upgrade
	std::shared_ptr<Sprite2D>			m_bgUpgrade;
	bool								m_isUpgrading;
	std::vector<std::shared_ptr<Button>> m_buttonUpgradeList;

	std::vector<std::shared_ptr<Text>>	m_textCoinList;
	GLint								m_totalCoinCost;
	// Text
	std::vector<std::shared_ptr<Text>>	m_textList;
	std::vector<std::shared_ptr<Sprite2D>> m_sprite2DList;

	// End game
	std::vector<std::shared_ptr<Button>> m_buttonEndGameList;
	std::vector<std::shared_ptr<Sprite2D>> m_sprite2DEndGameList;
	std::shared_ptr<Text> m_textTimePlay;

private:
	void LoadMapObstabcles(const std::string& filename);
	void Update2DDrawSize();
	void Update2DDrawPosition();
	void CreateBullet(TypeBullet typeBullet, FixtureTypes typeFixture, b2Vec2 speed, GLfloat posX, GLfloat posY, GLint currentDirection);
	void CreateObsDynamic(GLint IDTexture, GLfloat posX, GLfloat posY, GLfloat  width, GLfloat height, GLfloat top, GLfloat down);
	void UpdateObsDynamic(GLfloat deltaTime);
	void CreateBars(GLfloat posX, GLfloat posY, GLfloat height);
	void UpdateBars();
	std::shared_ptr<Button> CreateButton(GLint idTexture, GLfloat width, GLfloat height, GLfloat posX, GLfloat posY, ButtonType buttonType);
	void CreateText();
	void CreateWhenEndGame();
	void UpdateGameObjects();
};
