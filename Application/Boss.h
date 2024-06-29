#include <GLES2/gl2.h>
#include <memory>
#include "ResourcesManager.h"
#include "SpriteAnimation.h"
#include "box2d.h"
#include "Globals.h"
#include "Player.h"
#include "b2Utilities.h"

constexpr auto DETECTION_DISTANCE_SKILL1 = 2;
constexpr auto DETECTION_DISTANCE_SKILL2 = 4;
constexpr auto DETECTION_DISTANCE_ATTACK = 7;
constexpr auto HP_BOSS_MAX = 100;

enum BossActionType
{
	DASH_BOSS = 31,
	DEATH_BOSS,
	GET_HIT_BOSS,
	IDLE_BOSS,
	JUMP_BOSS,
	RUN_BOSS,
	SLASH_BOSS
};

class Boss
{
public:
	Boss();
	~Boss();

	void Update(float deltaTime, std::shared_ptr<Player> player);
	void Draw();

	void TakeDamage(GLint damage);

	void Set2DSizeByTile(GLint width, GLint height);
	void SetAction(BossActionType typeAction);
	void OnMouseScroll();

	Vector2				Get2DSize();
	inline b2Body*		GetBody() { return m_body; };
	inline b2Fixture*	GetEnemyFixture() { return m_bossFixture; };
	inline std::shared_ptr<SpriteAnimation> GetAnimation() { return m_animation; };
	inline GLint GetDamage() { return m_damage; };

private:
	GLint		m_health;
	GLint		m_damage;
	GLint		m_typeAction;
	bool		m_isAttack;
	bool		m_isDie;
	bool		m_isNewTexture;
	bool		m_isActive;
	GLint		m_currentDirection;

	std::shared_ptr<Sprite2D> m_hpBar;
	std::shared_ptr<SpriteAnimation> m_animation;

	// Box 2D
	b2Body*		m_body;
	b2Body*		m_skillBossBody;
	b2Fixture*	m_bossFixture;
	b2Fixture*	m_skillBossFixture;
	Vector2		m_boxSize;
	Vector2		m_imgSize;
	Vector2		m_sizeByTile;

	// Skill
	GLfloat		m_coolDownSkill1, m_coolDownSkill2, m_coolDownIDLE, m_coolDownJump;

	// Effect
	std::shared_ptr<SpriteAnimation>	m_blood;
	bool								m_isTakeDamage;

private:
	void SetSizeInfo(Vector2 imgSize, Vector2 boxSize);
	void CreateBossBox();
	void AttackUpdate(b2Body* playerBody, GLfloat deltaTime);
	void CreateSkillFixture(Vector2 sizeSkillBox);
	void Kill();
};