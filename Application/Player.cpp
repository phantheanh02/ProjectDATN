#include "stdafx.h"
#include "Player.h"
#include "ResourcesManager.h"
#include "b2Utilities.h"
#include "Globals.h"
#include "Bullet.h"
#include "SceneManager.h"

extern int tileSizeByPixel;
extern CharacterType currentCharacter;
extern std::shared_ptr<b2World> world;

Player::Player()
	: m_currentDirection(DirectionType::RIGHT)
	, m_sprinningDirection(DirectionType::RIGHT)
	, m_currentAction(PlayerAction::IDLE)
	, m_isJumping(false)
	, m_bulletCooldown(0)
	, m_isDie(false)
	, m_resetAfterDieTime(5)
	, m_health(100)
	, m_jumpCooldown(0)
	, m_contacCount(0)
	, m_isTakeDamage(false)
	, m_isLoadingBullet(false)
{
	SetCharacter(currentCharacter);

	// create player body
	b2BodyDef playerBodyDef;
	playerBodyDef.type = b2_dynamicBody;
	playerBodyDef.position.Set(9.0f, 18.0f);
	playerBodyDef.userData.pointer = (uintptr_t)this;
	m_body = world->CreateBody(&playerBodyDef);

	// set player body properties
	b2PolygonShape playerShape;
	playerShape.SetAsBox(0.5 * SCALE_SIZE / 2, 2 / 3.0f * SCALE_SIZE / 2); // size box = size img / 2
	b2FixtureDef playerFixtureDef;
	playerFixtureDef.shape = &playerShape;
	playerFixtureDef.density = 1.0f; 
	playerFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER;
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_ENEMY_BULLET | FixtureTypes::FIXTURE_ENEMY | FixtureTypes::FIXTURE_ITEM;
	playerFixtureDef.friction = 0.3f; // Adjusted friction
	m_body->CreateFixture(&playerFixtureDef);
	m_body->SetFixedRotation(true); 
	// Create sensor foot to check in ground
	b2PolygonShape sensorBox;
	b2FixtureDef sensorDef;
	sensorBox.SetAsBox(0.5f, 0.1f, b2Vec2(0, 1), 0); // sensor foot
	sensorDef.shape = &sensorBox;
	sensorDef.isSensor = true;
	sensorDef.filter.categoryBits = FixtureTypes::FIXTURE_PLAYER_FOOT;
	sensorDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND;
	sensorDef.userData.pointer = (uintptr_t)this;
	m_body->CreateFixture(&sensorDef);
	
	// animation
	Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
	m_actionAnimation = std::make_shared<SpriteAnimation>(m_currentCharacter, 5, 0.1);
	m_actionAnimation->Set2DPositionByTile(3, 5);
	m_actionAnimation->Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);

	m_blood = std::make_shared<SpriteAnimation>(111, 18, 0.02f);
	
	// HUB
	// HP bar
	m_HPBar  = std::make_shared<Sprite2D>(109);
	m_HPBar->Set2DSizeByTile(1.5, 0.1f);

	// Create bullet pooling
	for (int i = 0; i < 100; i++)
	{
		m_bulletList.push_back(std::make_shared<Bullet>(world.get()));
	}
}

Player::~Player()
{
}

void Player::Update(float deltaTime)
{
	for (auto it : m_bulletList)
	{
		it->Update(deltaTime);
	}

	if (m_jumpCooldown > 0)
	{
		m_jumpCooldown -= deltaTime;
	}

	if (m_isDie)
	{
		m_resetAfterDieTime -= deltaTime;
		SetAction(PlayerAction::DEAD);
		//m_body->SetEnabled(false);
		if (m_actionAnimation->GetCurrentFrame() != m_actionAnimation->GetNumFrames() - 1)
		{
			m_actionAnimation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
			m_actionAnimation->Update(deltaTime);
		}
		return;
	}
	if (!m_isJumping)
	{
		m_actionAnimation->Update(deltaTime);
	}
	m_actionAnimation->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);
	if (m_bulletCooldown > 0)
	{
		m_bulletCooldown -= deltaTime;
	}

	// Update HUB
	auto pos = m_body->GetPosition();
	m_HPBar->Set2DPositionByTile(pos.x - 0.7, pos.y - 1.5);

	// handle when take damage
	if (m_isTakeDamage)
	{
		m_blood->Update(deltaTime);
		if (m_blood->IsLastFrame())
		{
			m_isTakeDamage = false;
		}
	}

	if (m_isLoadingBullet > 0)
	{
		m_isLoadingBullet -= deltaTime;
		if (m_isLoadingBullet <= 0)
		{
			m_numberBullet = m_stats.numberBullet;
		}
	}
}

void Player::Draw()
{
	for (auto it : m_bulletList)
	{
		it->Draw();
	}

	m_HPBar->Draw();
	m_actionAnimation->Draw();
	if (m_isTakeDamage)
	{
		m_blood->Draw();
	}
}

void Player::SetCurrentDirectionByPreDirection()
{
	m_currentDirection = m_sprinningDirection;
}

void Player::Set2DPositionByTile(GLfloat x, GLfloat y)
{
	m_pos.x = x;
	m_pos.y = y;
}

void Player::Set2DSizeByTile(GLfloat width, GLfloat height)
{
	m_size.x = width;
	m_size.y = height;
}

void Player::SetDirection(DirectionType direction)
{
	if (m_currentDirection != direction)
	{
		if (direction != DirectionType::TOP && direction != m_sprinningDirection)
		{
			m_actionAnimation->FlipVertical();
			m_sprinningDirection = direction;
		}
		m_currentDirection = direction;
	}
}

void Player::SetAction(PlayerAction action)
{
	if (m_currentAction != action && m_currentAction != DEAD)
	{
		auto pos = m_actionAnimation->GetPosition();
		auto size = m_actionAnimation->GetSize();
		auto model = m_actionAnimation->GetModel();
		switch (action)
		{
		case IDLE:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 5, 0.1f);
			break;
		case RUNNING:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 6, 0.1f);
			break;
		case JUMPING:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 2, 0.1f);
			break;
		case CROUCH:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 3, 0.1f);
			break;
		case DEAD:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 8, 0.1f);
			m_actionAnimation->SetCurrentFrame(0);
			break;
		case FIRE_TOP:
			m_actionAnimation = std::make_shared<SpriteAnimation>(action + m_currentCharacter, 1, 0.1f);
			break;
		case NONE_ACTION:
			break;
		default:
			break;
		}
		m_actionAnimation->Set2DPosition(pos.x, pos.y);
		m_actionAnimation->Set2DSize(size.x, size.y);
		m_actionAnimation->SetModel(model);
		m_currentAction = action;
		if (action == PlayerAction::JUMPING)
		{
			m_actionAnimation->SetCurrentFrame(m_actionAnimation->GetNumFrames() - 1); // Set is final frame
		}
	}
}

void Player::SetJumpingStatus(bool status)
{
	m_isJumping = status;
}

void Player::SetCharacter(CharacterType type)
{
	auto stats = SceneManager::GetInstance()->GetCharacterStats(type);
	if (stats.type)
	{
		m_stats = stats;
		m_health = stats.hp;
		m_numberBullet = stats.numberBullet;
	}

	m_currentCharacter = type;
	switch (m_currentCharacter)
	{
	case C_BLACK:
		m_playerbulletType = 1;
		break;
	case C_BLUE:
		m_playerbulletType = 2;
		break;
	case C_GREEN:
		m_playerbulletType = 3;
		break;
	case C_RED:
		m_playerbulletType = 4;
		break;
	case C_YELLOW:
		m_playerbulletType = 5;
		break;
	default:
		break;
	}
}

void Player::SetLoadingBullet(GLfloat time)
{
	m_isLoadingBullet = time; 

	// if reset time then reset number bullet
	if (m_isLoadingBullet == 0)
	{
		m_numberBullet = m_stats.numberBullet;
	}
}

void Player::GetItem(GLint typeItem)
{
	
}

bool Player::IsReadyJump()
{
	return (m_jumpCooldown <= 0) && (!m_isJumping) && (m_currentAction != PlayerAction::JUMPING);
}

void Player::HandleEvent(int event)
{
	// Handle Key
	auto currentVelocity = m_body->GetLinearVelocity();
	float desiredVel = 0, velChange = 0, impulse = 0;

	if (event & (1 << 1) )
	{
		// move left
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		SetDirection(DirectionType::LEFT);
	}
	if (event & (1 << 3))
	{
		// move right
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		SetDirection(DirectionType::RIGHT);
	}
	if (desiredVel != 0 && !m_isJumping)
	{
		SetAction(PlayerAction::RUNNING);
	}
	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_body->GetMass() * velChange; //disregard time factor

	m_body->ApplyLinearImpulse(b2Vec2(impulse, 0), m_body->GetWorldCenter(), true);

	if (event & (1 << 0) && velChange == 0 && !m_isJumping)
	{
		// key w
		SetAction(PlayerAction::FIRE_TOP);
		SetDirection(DirectionType::TOP);
	}
	else if (m_currentDirection != m_sprinningDirection)
	{
		m_currentDirection = m_sprinningDirection;
	}


	if (event & (1 << 2))
	{
		// key s
		SetAction(PlayerAction::CROUCH);
	}

	// JUMP: key space
	if (event & (1 << 4) && IsReadyJump())
	{
		SetAction(PlayerAction::JUMPING);
		desiredVel = b2Min(currentVelocity.x, currentVelocity.y - 3.0f);
		float jumpHeight = 5.0f;
		impulse = m_body->GetMass() * sqrt(2 * world->GetGravity().y * jumpHeight);
		m_jumpCooldown = JUMP_COOLDOWN;
		m_body->ApplyLinearImpulse(b2Vec2(0, -impulse), m_body->GetWorldCenter(), true);
		//m_body->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
	}

	if (!(event & 0xF))
	{
		// none of movement key is pressed
		SetAction(PlayerAction::IDLE);
	}
	if (m_isJumping)
	{
		SetAction(PlayerAction::JUMPING);
	}

	// Fire
	if (event & (1 << 5) && m_bulletCooldown <= 0 && m_isLoadingBullet <= 0 )
	{
		m_bulletCooldown = BULLET_COOLDOWN;

		b2Vec2 pos = m_body->GetPosition();
		b2Vec2 speed;

		switch (m_currentDirection)
		{
		case DirectionType::TOP:
			speed = b2Vec2(0.0f, -20.0f);
			break;
		case DirectionType::LEFT:
			speed = b2Vec2(-20.0f, 0.0f);
			break;
		case DirectionType::RIGHT:
			speed = b2Vec2(20.0f, 0.0f);
			break;
		}
		CreateBullet(m_playerbulletType, speed, Vector2(m_body->GetPosition().x, m_body->GetPosition().y));

		m_numberBullet--;
		if (m_numberBullet <= 0)
		{
			m_isLoadingBullet = LOADING_BULLET_TIME;
		}
	}
}

void Player::TakeDamage(GLint damage)
{
	m_isTakeDamage = true;
	m_blood->SetCurrentFrame(0);
	m_blood->Set2DSizeByTile(3, 3);
	m_blood->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);

	m_health = m_health <= 0 ? 0 : m_health - damage;
	if (m_health <= 0)
	{
		m_isDie = true;
		m_isTakeDamage = false;
	}

	m_HPBar->Set2DSizeByTile(1.5f * m_health / m_stats.hp, 0.1f);
	
}

void Player::ReCalculateWhenScroll()
{
	Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
	m_actionAnimation->Set2DSizeByTile(m_size.x, m_size.y);

	m_HPBar->Set2DSizeByTile(1.5f * m_health / m_stats.hp, 0.1f);
}

void Player::HandlePlayerDie(GLfloat deltaTime)
{
}

void Player::CreateBullet(int type, b2Vec2 speed, Vector2 position)
{
	for (auto bullet : m_bulletList)
	{
		if (!bullet->IsActive())
		{
			bullet->CreateNewBullet((BulletType)type, speed, position);
			break;
		}
	}
}
