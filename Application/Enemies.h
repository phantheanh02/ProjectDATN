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

class Enemies
{
public:
	Enemies(GLint id);
	Enemies(GLint id, GLint idTexture);
	~Enemies();

	void Update(float deltaTime, std::shared_ptr<Player> player);
	void Draw();
	void Move();
	void Intro();
	void Set2DPosition(GLint x, GLint y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeScroll();
	Vector2 Get2DSize();
	void SetTexture(GLint typeAction);
	void Set2DPositionFromBox2D(GLfloat x, GLfloat y);
	std::shared_ptr<SpriteAnimation> GetAnimation() { return m_animation; };
	
	void		CreateEnemyBox(std::shared_ptr<b2World> world, float x, float y);
	b2Body*		GetEnemyBody();
	b2Fixture*	GetEnemyFixture();

	void CalculateSize();
	void SetBoundaryMove(GLfloat x, GLfloat y);

	inline bool IsActive() { return m_isActive; };
	inline bool IsDie() { return m_isDie; };
	inline GLint CurrentDirection() { return m_currentDirection; }
	inline void SetTileSize(GLint tileSize) { m_tileSizeByPixel = tileSize; };

	void		TakeDamage(GLint damage);
	void		Kill();

	GLint		typeEnemeis;
	BoxEnemy	m_boxInfo; 
	GLfloat		m_coolDown;
	bool		m_isTakeCoin;
private:
	GLint		m_id;
	GLint		m_health;
	GLfloat		m_fSpeed;
	GLfloat		m_timeDie;
	std::unordered_map<GLint, GLint> m_textureList;
	std::shared_ptr<SpriteAnimation> m_animation;
	std::shared_ptr<Sprite2D> m_hpBar;
	GLfloat		m_timeBarDisplay;

	bool		m_isActive;
	bool		m_isDie;
	bool		m_isChange;
	bool		m_isFire;

	b2Body*		m_enemyBody;
	b2Fixture*	m_enemyFixture;
	Vector2		sizeBox;
	Vector2		size2D;
	GLint		m_currentDirection; // 1 - Right; -1 Left;

	MyRayCastCallback m_rayCallback;
	Vector2		m_boundaryMove;
	GLint		m_tileSizeByPixel = tileSizeByPixel;
	GLint		typeAttack;
private:
	void		PerformRayCasting();
	void		RunModUpdate(std::shared_ptr<Player> player);
	void		FindPlayerUpdate(std::shared_ptr<Player> player);
	void		AttackUpdate(std::shared_ptr<Player> player);
	void		EnemyFlyUpdate(std::shared_ptr<Player> player);
};