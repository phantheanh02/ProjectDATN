#include <GLES2/gl2.h>
#include <memory>
#include "ResourcesManager.h"
#include "SpriteAnimation.h"
#include "box2d.h"
#include "Globals.h"
#include "ConfigClass.h"
#include "Player.h"
#include "b2Utilities.h"

constexpr auto DETECTION_DISTANCE_SKILL1 = 2;
constexpr auto DETECTION_DISTANCE_SKILL2 = 4;
constexpr auto DETECTION_DISTANCE_ATTACK = 7;
constexpr auto HP_BOSS_MAX = 15;

class Boss
{
public:
	Boss(b2World* world);
	~Boss();

	void Update(float deltaTime, std::shared_ptr<Player> player);
	void Draw();

	void TakeDamage(GLint damage);

	void Set2DPosition(GLint x, GLint y);
	void Set2DSize(GLint width, GLint height);
	void Set2DSizeByTile();
	void SetTexture(GLint typeAction);
	void Set2DPositionFromBox2D(GLfloat x, GLfloat y);
	void SetTileSizeScroll(GLint tileSize);

	Vector2				Get2DSize();
	inline b2Body*		GetBody() { return m_bossBody; };
	inline b2Fixture*	GetEnemyFixture() { return m_bossFixture; };
	inline std::shared_ptr<SpriteAnimation> GetAnimation() { return m_animation; };
	inline GLint GetDamage() { return m_damage; };
	GLint m_typeAction;
	bool		m_isAttack;
	bool		m_isDie;

private:
	GLint		m_health;
	GLint		m_damage;
	GLint		m_tileSizeByPixel;
	std::shared_ptr<SpriteAnimation> m_animation;
	bool		m_isNewTexture;
	bool		m_isActive;
	Vector2		m_pos;
	GLint		m_currentDirection;
	std::shared_ptr<Sprite2D> m_hpBar;
	GLfloat		m_timeBarDisplay;

	// Box 2D
	b2Body*		m_bossBody;
	b2Body*		m_skillBossBody;
	b2Fixture*	m_bossFixture;
	b2Fixture*	m_skillBossFixture;
	Vector2		m_imgSize;
	Vector2		m_boxSize;

	// Skill
	GLfloat		m_coolDownSkill1, m_coolDownSkill2, m_coolDownIDLE, m_coolDownJump;
private:
	void SetBoxInfo(Vector2 imgSize, Vector2 boxSize);
	void CreateBossBox(b2World* world);
	void AttackUpdate(b2Body* playerBody, GLfloat deltaTime);
	void CreateSkillFixture(Vector2 sizeSkillBox);
	void DestroyFixture(b2Fixture* fixture);
	void Kill();
};