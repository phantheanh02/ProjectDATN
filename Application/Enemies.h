#pragma once
#include <GLES2/gl2.h>
#include "../Utilities/Math.h"
#include <memory>
#include "SpriteAnimation.h"
#include <unordered_map>
#include "box2d.h"
#include "Player.h"
#include "b2Utilities.h"
#include "Sprite2D.h"
#include "SceneManager.h"
#include "ConfigClass.h"
#include "Globals.h"

enum EnemyAction
{
	FIND_PLAYER = 1,
	RUN = 17,
	FIRE_DOWN,
	FIRE_UP,
	DIE,
	ARMOD_FIRE,
	RPGMOD_FIRE,
	SNIPERMOD_FIRE,
	ENEMY_01 = 38,
	ENEMY_02 = 39,
	ENEMY_03 = 40,
	ENEMY_EXPLOSION = 41,
	ACTION_INVALID = -1
};

enum EnemyType
{
	AR_MOD,
	RPG_MOD,
	Sniper_MOD,
	PATREON,
	MEGAMAN,
	YUME
};

class Enemies
{
public:
	Enemies(EnemyType type);
	Enemies(GLint id, GLint idTexture);
	~Enemies();

	void Update(float deltaTime, std::shared_ptr<Player> player);
	void Draw();
	void Move();
	void Init();

	void CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y);
	void CalculateSize();
	void TakeDamage(GLint damage);
	void Kill();

	void Set2DPosition(GLint x, GLint y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeScroll();
	void SetTexture(GLint typeAction);
	void Set2DPositionFromBox2D(GLfloat x, GLfloat y);
	void SetBoundaryMove(GLfloat x, GLfloat y);

	Vector2								Get2DSize();
	std::shared_ptr<SpriteAnimation>	GetAnimation() { return m_animation; };
	b2Body*								GetEnemyBody();
	b2Fixture*							GetEnemyFixture();
	inline DirectionType				GetCurrentDirection() { return m_currentDirection; }
	inline bool							IsActive() { return m_isActive; };
	inline bool							IsDie() { return m_isDie; };


private:
	// Attribute
	GLint			m_id; // ID texture
	EnemyType		m_type;
	EnemyAction		m_currentAction;
	GLint			m_health;
	GLfloat			m_speed;
	GLfloat			m_coolDown;
	DirectionType	m_currentDirection;

	// Sprite
	std::shared_ptr<SpriteAnimation> m_animation;
	std::shared_ptr<Sprite2D> m_hpBar;
	GLfloat		m_timeBarDisplay;

	// Status
	bool		m_isActive;
	bool		m_isDie;
	bool		m_isChange;
	bool		m_isFire;

	// box2d
	b2Body*		m_body;
	b2Fixture*	m_fixture;
	MyRayCastCallback m_rayCallback;

	
private:
	void		PerformRayCasting();
	void		RunModUpdate(std::shared_ptr<Player> player);
	void		FindPlayerUpdate(std::shared_ptr<Player> player);
	void		AttackUpdate(std::shared_ptr<Player> player);
	void		EnemyFlyUpdate(std::shared_ptr<Player> player);
};