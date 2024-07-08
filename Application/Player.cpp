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
	, m_resetAfterDieTime(3)
	, m_health(100)
	, m_jumpCooldown(0)
	, m_contacCount(0)
	, m_isTakeDamage(false)
	, m_isLoadingBullet(false)
	, m_coin(1000)
	, m_armor(0)
	, m_isOpponentCharacter(false)
	, m_armorActiveTime(0)
	, m_isHasCup(false)
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
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_ENEMY_BULLET | FixtureTypes::FIXTURE_BOSS_BULLET | FixtureTypes::FIXTURE_ENEMY | FixtureTypes::FIXTURE_ITEM;
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
	m_armorSprite = std::make_shared<Sprite2D>("Item/item_armor.png");
	m_armorSprite->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));

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

Player::Player(CharacterType currentOpponentCharacter)
	: m_currentDirection(DirectionType::RIGHT)
	, m_sprinningDirection(DirectionType::RIGHT)
	, m_currentAction(PlayerAction::IDLE)
	, m_isJumping(false)
	, m_bulletCooldown(0)
	, m_isDie(false)
	, m_resetAfterDieTime(3)
	, m_health(100)
	, m_jumpCooldown(0)
	, m_contacCount(0)
	, m_isTakeDamage(false)
	, m_isLoadingBullet(false)
	, m_coin(0)
	, m_armor(0)
	, m_isOpponentCharacter(true)
	, m_armorActiveTime(0)
{
	SetCharacter(currentOpponentCharacter);

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
	playerFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_ENEMY;
	playerFixtureDef.filter.maskBits = FixtureTypes::FIXTURE_GROUND | FixtureTypes::FIXTURE_PLAYER_BULLET | FixtureTypes::FIXTURE_ITEM;
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
	m_armorSprite = std::make_shared<Sprite2D>("Item/item_armor.png");
	m_armorSprite->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));

	// HUB
	// HP bar
	m_HPBar = std::make_shared<Sprite2D>(109);
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

	if (m_armorActiveTime > 0)
	{
		m_armorActiveTime -= deltaTime;
		m_armorSprite->Set2DPositionByTile(pos.x, pos.y);
		m_armorSprite->SetTransparency(m_armorActiveTime / ARMOR_ACTIVE_TIME);
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
	if (m_armorActiveTime > 0)
	{
		m_armorSprite->Draw();
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
		m_stats.hp *= 10;
		m_health = m_stats.hp;
		m_numberBullet = m_stats.numberBullet;
	}

	m_currentCharacter = type;
	switch (m_currentCharacter)
	{
	case C_BLACK:
		m_playerbulletType = BLACK_BULLET;
		break;
	case C_BLUE:
		m_playerbulletType = BLUE_BULLET;
		break;
	case C_GREEN:
		m_playerbulletType = GREEN_BULLET;
		break;
	case C_RED:
		m_playerbulletType = RED_BULLET;
		break;
	case C_YELLOW:
		m_playerbulletType = YELLOW_BULLET;
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

void Player::GetItem(ItemType typeItem)
{
	ResourcesManager::GetInstance()->GetSound(11)->Play();

	switch (typeItem)
	{
	case HEALING:
		m_health += 5;
		m_health = m_health > m_stats.hp ? m_stats.hp : m_health;
		m_HPBar->Set2DSizeByTile(1.5f * m_health / m_stats.hp, 0.1f);
		break;
	case COIN:
		m_coin += 100;
		break;
	case ARMOR:
		m_armor++;
		break;
	case CUP:
		m_isHasCup = true;
		break;
	default:
		break;
	}
}

bool Player::IsReadyJump()
{
	return (m_jumpCooldown <= 0) && (!m_isJumping) && (m_currentAction != PlayerAction::JUMPING);
}

void Player::HandleEvent(int event)
{
	// Handle Key
	auto currentVelocity = m_body->GetLinearVelocity();
	float desiredVel = 0, velChange = 0, impulseMove = 0, impulseJump = 0;

	// MOVE LEFT
	if (event & (1 << 1) )
	{
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		SetDirection(DirectionType::LEFT);
	}
	// MOVE RIGHT
	if (event & (1 << 3))
	{
		// move right
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		SetDirection(DirectionType::RIGHT);
	}
	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulseMove = m_body->GetMass() * velChange; //disregard time factor
	m_body->ApplyLinearImpulse(b2Vec2(impulseMove, 0), m_body->GetLocalCenter(), true);

	// JUMP: key space
	if (event & (1 << 4) && IsReadyJump())
	{
		SetAction(PlayerAction::JUMPING);
		desiredVel = b2Min(currentVelocity.x, currentVelocity.y - 3.0f);
		float jumpHeight = 5.0f;
		impulseJump = m_body->GetMass() * sqrt(2 * world->GetGravity().y * jumpHeight);
		m_jumpCooldown = JUMP_COOLDOWN;
		m_body->ApplyLinearImpulse(b2Vec2(0, -impulseJump), m_body->GetWorldCenter(), true);
	}

	
	if (desiredVel != 0 && !m_isJumping)
	{
		SetAction(PlayerAction::RUNNING);
	}

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
		CreateBullet(speed);
		ResourcesManager::GetInstance()->GetSound(4)->Play();

		m_numberBullet--;
		if (m_numberBullet <= 0)
		{
			m_isLoadingBullet = LOADING_BULLET_TIME;
		}
	}
}

void Player::TakeDamage(GLint damage)
{
	if (m_armorActiveTime > 0)
	{
		return;
	}

	m_isTakeDamage = true;
	m_blood->SetCurrentFrame(0);
	m_blood->Set2DSizeByTile(3, 3);
	m_blood->Set2DPositionByTile(m_body->GetPosition().x, m_body->GetPosition().y);

	m_health = m_health <= 0 ? 0 : m_health - damage;
	if (m_health <= 0)
	{
		m_isDie = true;
		m_isTakeDamage = false;
		ResourcesManager::GetInstance()->GetSound(5)->Play();
	}
	else
	{
		ResourcesManager::GetInstance()->GetSound(7)->Play();
	}
	m_HPBar->Set2DSizeByTile(1.5f * m_health / m_stats.hp, 0.1f);
	
}

void Player::ReCalculateWhenScroll()
{
	Set2DSizeByTile(SCALE_SIZE, SCALE_SIZE);
	m_actionAnimation->Set2DSizeByTile(m_size.x, m_size.y);

	m_HPBar->Set2DSizeByTile(1.5f * m_health / m_stats.hp, 0.1f);
	for (auto bullet : m_bulletList)
	{
		if (bullet->IsActive())
		{
			bullet->OnMouseScroll();
		}
	}
}

void Player::ActiveArmorItem()
{
	if (m_armor > 0)
	{
		m_armor--;
		m_armorActiveTime = ARMOR_ACTIVE_TIME;
		m_armorSprite->Set2DSizeByTile(1, 1);
		m_armorSprite->SetTransparency(1);
	}
}

void Player::HandlePlayerDie(GLfloat deltaTime)
{
}

void Player::CreateBullet(b2Vec2 speed)
{
	for (auto bullet : m_bulletList)
	{
		if (!bullet->IsActive())
		{
			bullet->CreateNewBullet((BulletType)m_playerbulletType, speed, Vector2(m_body->GetPosition().x, m_body->GetPosition().y), m_stats.atk, m_isOpponentCharacter);
			break;
		}
	}
}
