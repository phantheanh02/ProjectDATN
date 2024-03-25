#include "stdafx.h"
#include <fstream>
#include <Windows.h>
#include "GSPlay.h"
#include "ResourcesManager.h"
#include "Globals.h"
#include "GameStateMachine.h"
#include "SceneManager.h"

// Bug
// Text size

GSPlay::~GSPlay()
{
}

void GSPlay::Init()
{
	m_dynamicCamera = SceneManager::GetInstance()->GetCamera(0);
	m_staticCamera = SceneManager::GetInstance()->GetCamera(1);
	m_key = 0;
	m_lastJumpTime = 0.0f;
	m_lastShootTime = 0.0f;
	m_needUpdateDrawSize = true;
	m_lastPlayerHorizontalDirection = D_RIGHT;
	m_numberEnemyKill = 0;
	m_isUpgrading = false;
	m_totalCoinCostBullet = 0;
	m_totalCoinCostArmor = 0;
	m_totalCoinCostHP = 0;	
	m_totalCoinCost = 0;
	m_isBossDie = true;
	m_isPlayerDie = false;
	m_timeReplay = 5.0f;
	m_isOnDynamicObs = false;
	m_timePlay = 0.0f;
	// get refresh rate and set time step
	DEVMODE devMode;
	EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &devMode);
	m_timeStep = 1.0f / devMode.dmDisplayFrequency;

	// Box2d
	m_gravity = {0.0f, 20.0f};
	// create world with gravity
	m_world = std::make_shared<b2World>(m_gravity);
	m_contactListener = new ContactListener();
	m_world->SetContactListener(m_contactListener);

	// Player
	m_player = std::make_shared<Player>(m_world.get());
	m_player->Set2DSize(m_tileSizeByPixel * 9.0f / 4, m_tileSizeByPixel * 9.0f / 4);

	// Boss
	m_boss = std::make_shared<Boss>(m_world.get());

	// init camera boundaries
	LoadMapObstabcles("map1.txt");
	m_cameraPositionBoudaries.x = 0;
	m_cameraPositionBoudaries.y = 0;
	m_cameraPositionBoudaries.z = m_mapSizeByTile.x * m_tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = m_mapSizeByTile.y * m_tileSizeByPixel - (GLfloat)Globals::screenHeight;


	// Bullet
	for (int i = 0; i < 100; i++)
	{
		auto bullet = std::make_shared<Bullet>(m_world.get());
		m_bulletList.push_back(bullet);
	}

	// sound
	auto sound = ResourcesManager::GetInstance()->GetSound(0);
	sound->Play(-1);

	// Upgrade
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(50);

	m_bgUpgrade = std::make_shared<Sprite2D>(0, model, shader, texture);
	m_bgUpgrade->AttachCamera(m_staticCamera);
	m_bgUpgrade->Set2DSize(483 / 25.0f * m_tileSizeByPixel, 243 / 25.0f * m_tileSizeByPixel);
	m_bgUpgrade->Set2DPosition(Globals::screenWidth / 2.0f - m_bgUpgrade->Get2DSize().x / 2.0f, Globals::screenHeight / 2.0f - m_bgUpgrade->Get2DSize().y / 2.0f);
	// Button upgrade
	// Button buy bullet
	Vector2 size = Vector2(78 / 25.0f * m_tileSizeByPixel, 36 / 25.0f * m_tileSizeByPixel);
	GLfloat posX = m_bgUpgrade->Get2DPosition().x + 214 / 25.0f * m_tileSizeByPixel;
	GLfloat posY = m_bgUpgrade->Get2DPosition().y + 50 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(51, size.x, size.y, posX, posY, BUTTON_BUY_BULLET_UPGRADE));
	// Button buy armor
	posY = m_bgUpgrade->Get2DPosition().y + 117 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(51, size.x, size.y, posX, posY, BUTTON_BUY_ARMOR_UPGRADE));
	// Button buy hp
	posY = m_bgUpgrade->Get2DPosition().y + 182 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(51, size.x, size.y, posX, posY, BUTTON_BUY_HP_UPGRADE));
	// Button exit
	size = Vector2(20 / 25.0f * m_tileSizeByPixel, 19 / 25.0f * m_tileSizeByPixel);
	posX = m_bgUpgrade->Get2DPosition().x + 278 / 25.0f * m_tileSizeByPixel;
	posY = m_bgUpgrade->Get2DPosition().y + 9 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(52, size.x, size.y, posX, posY, BUTTON_EXIT_UPGRADE));
	// Button plus on bullet
	size = Vector2(11 / 25.0f * m_tileSizeByPixel, 11 / 25.0f * m_tileSizeByPixel);
	posX = m_bgUpgrade->Get2DPosition().x + 185 / 25.0f * m_tileSizeByPixel;
	posY = m_bgUpgrade->Get2DPosition().y + 62 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(53, size.x, size.y, posX, posY, BUTTON_PLUS_ON_BULLET_UPGRADE));
	// Button plus on armor
	posY = m_bgUpgrade->Get2DPosition().y + 129 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(53, size.x, size.y, posX, posY, BUTTON_PLUS_ON_ARMOR_UPGRADE));
	// Button plus on hp
	posY = m_bgUpgrade->Get2DPosition().y + 194 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(53, size.x, size.y, posX, posY, BUTTON_PLUS_ON_HP_UPGRADE));
	// Button plus off bullet
	size = Vector2(11 / 25.0f * m_tileSizeByPixel, 5 / 25.0f * m_tileSizeByPixel);
	posX = m_bgUpgrade->Get2DPosition().x + 115 / 25.0f * m_tileSizeByPixel;
	posY = m_bgUpgrade->Get2DPosition().y + 65 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(54, size.x, size.y, posX, posY, BUTTON_PLUS_OFF_BULLET_UPGRADE));
	// Button plus off armor
	posY = m_bgUpgrade->Get2DPosition().y + 132 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(54, size.x, size.y, posX, posY, BUTTON_PLUS_OFF_ARMOR_UPGRADE));
	// Button plus off hp
	posY = m_bgUpgrade->Get2DPosition().y + 197 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(54, size.x, size.y, posX, posY, BUTTON_PLUS_OFF_HP_UPGRADE));
	// Button resum
	size = Vector2(110 / 25.0f * m_tileSizeByPixel, 35 / 25.0f * m_tileSizeByPixel);
	posX = m_bgUpgrade->Get2DPosition().x + 355 / 25.0f * m_tileSizeByPixel;
	posY = m_bgUpgrade->Get2DPosition().y + 45 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(72, size.x, size.y, posX, posY, BUTTON_RESUM));
	// Button retry
	posY = m_bgUpgrade->Get2DPosition().y + 90 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(73, size.x, size.y, posX, posY, BUTTON_RETRY));
	// Button menu
	posY = m_bgUpgrade->Get2DPosition().y + 135 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(74, size.x, size.y, posX, posY, BUTTON_MENU));
	// Button setting
	posY = m_bgUpgrade->Get2DPosition().y + 180 / 25.0f * m_tileSizeByPixel;
	m_buttonUpgradeList.push_back(CreateButton(75, size.x, size.y, posX, posY, BUTTON_SETTING));

	// Number Coin
	auto font = ResourcesManager::GetInstance()->GetFont(0);
	std::shared_ptr<Text> text = std::make_shared<Text>(0);
	text->AttachCamera(m_staticCamera);
	text->Init(font, "100");
	text->SetTextColor(152, 121, 36);
	text->Set2DScale(0.5f, 0.5f);
	posX = m_bgUpgrade->Get2DPosition().x + 140 / 25.0f * m_tileSizeByPixel;
	posY = m_bgUpgrade->Get2DPosition().y + 60 / 25.0f * m_tileSizeByPixel;
	text->Set2DPosition(posX, posY);
	m_textCoinList.push_back(text);

	// Number ARMOR
	text = std::make_shared<Text>(0);
	text->AttachCamera(m_staticCamera);
	text->Init(font, "100");
	text->SetTextColor(152, 121, 36);
	text->Set2DScale(0.5f, 0.5f);
	posY = m_bgUpgrade->Get2DPosition().y + 126 / 25.0f * m_tileSizeByPixel;
	text->Set2DPosition(posX, posY);
	m_textCoinList.push_back(text);

	// Number HP
	text = std::make_shared<Text>(0);
	text->AttachCamera(m_staticCamera);
	text->Init(font, "100");
	text->SetTextColor(152, 121, 36);
	text->Set2DScale(0.5f, 0.5f);
	posY = m_bgUpgrade->Get2DPosition().y + 192 / 25.0f * m_tileSizeByPixel;
	text->Set2DPosition(posX, posY);
	m_textCoinList.push_back(text);

	// Draw init to save position
	m_bgUpgrade->Draw();
	for (auto buttonUpgrade : m_buttonUpgradeList)
	{
		buttonUpgrade->Draw();
	}
	for (auto textInit : m_textCoinList)
	{
		textInit->Draw();
	}

	// Text
	CreateText();

	// End game
	CreateWhenEndGame();
}

void GSPlay::Update(float deltaTime)
{
	std::cout << "FPS: " << 1.0f / deltaTime << "\n";
	if (!m_isUpgrading && !m_player->m_isLost && !m_player->m_isVictory)
	{
		m_timePlay += deltaTime;
		m_totalCoinCost = m_totalCoinCostHP = m_totalCoinCostBullet = m_totalCoinCostArmor = 0;
		// step world
		m_world->Step(m_timeStep, VELOCITY_ITERATION, POSITION_ITERATION);

		HandleEvent();
		Update2DDrawPosition();
		if (m_needUpdateDrawSize)
		{
			Update2DDrawSize();
			m_needUpdateDrawSize = false;
		}

		// Update obs dynamic
		UpdateObsDynamic(deltaTime);
		UpdateBars();
		// Update enemy
		for (auto& it : m_enemiesList)
		{
			it->Update(deltaTime, m_player);
			if (it->IsDie() && !it->m_isTakeCoin)
			{
				m_player->m_coin += 50;
				it->m_isTakeCoin = true;
				m_numberEnemyKill++;
			}
		}

		// Update bullet
		for (auto& it : m_bulletList)
		{
			it->Update(deltaTime);
		}

		// Update item
		for (auto item : m_itemList)
		{
			item->Update(deltaTime);
		}


		// Update boss
		m_boss->Update(deltaTime, m_player);
		if (m_boss->m_isAttack)
		{
			m_isBossDie = false;
		}
		if (m_boss->m_isDie)
		{
			m_isBossDie = true;
		}
		// Update player
		m_lastJumpTime += deltaTime;
		m_lastShootTime -= deltaTime;
		m_player->Update(deltaTime);
		if (m_player->m_isPLayerDie)
		{
			m_timeReplay -= deltaTime;
			m_isPlayerDie = true;
		}
		if (m_timeReplay < 0)
		{
			// TODO: switch GS
		}
	}
	else if (m_isUpgrading)
	{
		m_textCoinList[0]->SetText(std::to_string(m_totalCoinCostBullet));
		m_textCoinList[1]->SetText(std::to_string(m_totalCoinCostArmor));
		m_textCoinList[2]->SetText(std::to_string(m_totalCoinCostHP));
	}
	else if (m_player->m_isLost)
	{

	}
	else if (m_player->m_isVictory)
	{
		if (m_timePlay < 120)
		{
			m_sprite2DEndGameList[1]->SetTexture(ResourcesManager::GetInstance()->GetTexture(78));
		}
		else if (m_timePlay < 160)
		{
			m_sprite2DEndGameList[1]->SetTexture(ResourcesManager::GetInstance()->GetTexture(77));
		}
		else 
		{
			m_sprite2DEndGameList[1]->SetTexture(ResourcesManager::GetInstance()->GetTexture(76));
		}
	}

	// Update text
	m_textList[0]->SetText(std::to_string(m_player->m_coin));
	m_textList[1]->SetText(std::to_string(m_player->m_health));
	m_textList[2]->SetText(std::to_string(m_player->m_armor));
	m_textList[3]->SetText(std::to_string(m_player->m_numberBullet));
	m_textList[4]->SetText(std::to_string(m_player->m_keyItem));

	// Update end game

}

void GSPlay::Draw()
{
	// Draw map
	if (m_player->m_isLost)
	{
		m_sprite2DEndGameList[0]->Draw();
		for (auto button : m_buttonEndGameList)
		{
			button->Draw();
		}
		m_textTimePlay->SetText(std::to_string((int)m_timePlay));
		m_textTimePlay->Draw();
	}
	else if (m_player->m_isVictory)
	{
		m_sprite2DEndGameList[1]->Draw();
		for (auto button : m_buttonEndGameList)
		{
			button->Draw();
		}
		m_textTimePlay->SetText(std::to_string((int)m_timePlay));
		m_textTimePlay->Draw();
	}
	else
	{
		m_map->Draw();
		for (auto& obs : m_obsDynamicList)
		{
			obs.sprite->Draw();
		}
		for (auto bars : m_barsList)
		{
			bars.sprite->Draw();
		}
		for (auto item : m_itemList)
		{
			item->Draw();
		}

		// Draw enemies
		for (auto& it : m_enemiesList)
		{
			if (it->IsActive())
			{
				it->Draw();
			}
		}

		// Draw bullet
		for (auto& it : m_bulletList)
		{
			if (!it->m_disabled)
			{
				it->Draw();
			}
		}

		// Draw boss
		m_boss->Draw();

		// Draw player
		m_player->Draw();

		if (m_isUpgrading)
		{
			// Upgrade
			m_bgUpgrade->Draw();
			for (auto buttonUpgrade : m_buttonUpgradeList)
			{
				buttonUpgrade->Draw();
			}
			for (auto text : m_textCoinList)
			{
				text->Draw();
			}
		}

		// Draw text
		for (auto& text : m_textList)
		{
			text->Draw();
		}
		for (auto sprite : m_sprite2DList)
		{
			sprite->Draw();
		}
	}


}

void GSPlay::Pause()
{
}

void GSPlay::Resume()
{
}

void GSPlay::Exit()
{
	for (auto& it : m_BodyList)
	{
		m_world->DestroyBody(it);
	}
	m_world.reset();
	delete m_contactListener;
}

void GSPlay::HandleEvent()
{
	auto currentVelocity = m_player->GetPlayerBody()->GetLinearVelocity();
	float desiredVel = 0, velChange, impulse;

	if (m_key & (1 << 1))
	{
		// move left
		m_player->m_iActionDirection = D_LEFT;
		m_player->m_iBotAction = RUNNING;
		desiredVel = b2Max(currentVelocity.x - 0.25f, -MOVEMENT_SPEED);
		m_lastPlayerHorizontalDirection = D_LEFT;
	}
	if (m_key & (1 << 3))
	{
		// move right
		m_player->m_iActionDirection = D_RIGHT;
		m_player->m_iBotAction = RUNNING;
		desiredVel = b2Min(currentVelocity.x + 0.25f, MOVEMENT_SPEED);
		m_lastPlayerHorizontalDirection = D_RIGHT;
	}
	if (m_key & (1 << 0))
	{
		// key w
		m_player->m_iActionDirection = D_UP;
	}
	if (m_key & (1 << 2))
	{
		// key s
		m_player->m_iActionDirection = D_DOWN;
	}
	if (!(m_key & 0xF))
	{
		// none of movement key is pressed
		m_player->m_iBotAction = IDLE;
		m_player->m_iActionDirection = m_lastPlayerHorizontalDirection;
		desiredVel = currentVelocity.x * 0.8f;
	}
	if (m_key & (1 << 5))
	{
		b2Vec2 pos = m_player->GetPlayerBody()->GetPosition();
		b2Vec2 speed;
		switch (m_player->m_iActionDirection)
		{
		case D_UP:
			speed = b2Vec2(0.0f, -20.0f);
			break;
		case D_DOWN:
			speed = b2Vec2(0.0f, 20.0f);
			break;
		case D_LEFT:
			speed = b2Vec2(-20.0f, 0.0f);
			break;
		case D_RIGHT:
			speed = b2Vec2(20.0f, 0.0f);
			break;
		}
		GLint currentDirec = m_player->m_flipped ? -1 : 1;
		if (m_player->m_numberBullet > 0 && m_lastShootTime <= 0)
		{
			m_player->m_iTopAction = SHOOTING;
			CreateBullet(PLAYER_BULLET, FIXTURE_PLAYER_BULLET, speed, pos.x, pos.y, currentDirec);
			ResourcesManager::GetInstance()->GetSound(4)->Play();
			m_lastShootTime = 0.2f;
			m_player->m_numberBullet--;
		}
		else if (m_player->m_numberBullet == 0 && m_lastShootTime <= 0)
		{
			ResourcesManager::GetInstance()->GetSound(6)->Play();
			m_lastShootTime = 0.2f;
		}
	}
	else
	{
		m_player->m_iTopAction = IDLE;
	}
	// check if on air
	if (!m_player->m_onGround)
	{
		m_player->m_iBotAction = Action::JUMPING;
	}

	// apply force to move
	velChange = desiredVel - currentVelocity.x;
	impulse = m_player->GetPlayerBody()->GetMass() * velChange; //disregard time factor
	m_player->GetPlayerBody()->ApplyLinearImpulse(b2Vec2(impulse, 0), m_player->GetPlayerBody()->GetWorldCenter(), true);

	if (m_key & (1 << 4) && m_lastJumpTime >= 0.5f && m_player->m_onGround)
	{
		// jump
		float jumpHeight = 3.0f;
		auto impulse = m_player->GetPlayerBody()->GetMass() * sqrt(2 * m_gravity.y * jumpHeight);
		m_player->GetPlayerBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -impulse), true);
		m_lastJumpTime = 0.0f;
	}

	// Enemy attack
	for (auto& enemy : m_enemiesList)
	{
		if (!enemy->IsDie() && enemy->typeEnemeis != RUN && enemy->typeEnemeis != FIND_PLAYER && enemy->m_coolDown <= 0)
		{
			b2Vec2 pos = enemy->GetEnemyBody()->GetPosition();
			b2Vec2 speed = enemy->CurrentDirection() * b2Vec2(10.0f, 0.0f);
			TypeBullet typeBullet;
			switch (enemy->typeEnemeis)
			{
			case ARMOD_FIRE:
				typeBullet = AR_BULLET;
				pos.y -= 0.1875f;
				break;
			case SNIPERMOD_FIRE:
				typeBullet = SNIPER_BULLET;
				pos.y -= 0.1875f;
				break;
			case RPGMOD_FIRE:
				typeBullet = RPG_BULLET;
				pos.y -= 0.1875f;
				break;
			case ENEMY_03:
			{
				b2Vec2 directionPlayerEnemy = m_player->GetPlayerBody()->GetPosition() - enemy->GetEnemyBody()->GetPosition();
				directionPlayerEnemy.Normalize();
				speed = b2Vec2(directionPlayerEnemy.x * 3.0f, directionPlayerEnemy.y * 3.0f);
				typeBullet = ENEMY_FLY_BULLET;
				pos.y += 1.0f;
				break;
			}
			default:
				break;
			}
			CreateBullet(typeBullet, FIXTURE_ENEMY_BULLET, speed, pos.x, pos.y, enemy->CurrentDirection());
			
			if (typeBullet == ENEMY_FLY_BULLET)
			{
				enemy->m_coolDown = 1.0f;
			}
			else
			{
				enemy->m_coolDown = enemy->GetAnimation()->TimeAction();
			}
		}
	}


	// mark collided bullets as disabled and remove
	for (auto& bullet : m_bulletList)
	{
		if (bullet->m_disabled)
		{
			bullet->GetBulletBody()->SetEnabled(false);
		}
	}
}

void GSPlay::OnKey(unsigned char key, bool pressed)
{
	if (pressed)
	{
		switch (key)
		{
		case KEY_W:
			m_key |= 1 << 0;
			break;
		case KEY_A:
			m_key |= 1 << 1;
			break;
		case KEY_S:
			m_key |= 1 << 2;
			break;
		case KEY_D:
			m_key |= 1 << 3;
			break;
		case KEY_SPACE:
			m_key |= 1 << 4;
			break;
		case KEY_J:
			m_key |= 1 << 5;
			break;
		case KEY_ESC:
			//GameStateMachine::GetInstance()->Exit();
			m_isUpgrading = m_isUpgrading == true ? false : true;
			break;
		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case KEY_W:
			m_key ^= 1 << 0;
			break;
		case KEY_A:
			m_key ^= 1 << 1;
			break;
		case KEY_S:
			m_key ^= 1 << 2;
			break;
		case KEY_D:
			m_key ^= 1 << 3;
			break;
		case KEY_SPACE:
			m_key ^= 1 << 4;
			break;
		case KEY_J:
			m_key ^= 1 << 5;
			break;
		case KEY_ESC:
			break;
		default:
			break;
		}
	}
}

void GSPlay::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	if (pressed)
	{
		switch (key)
		{
		case MOUSE_LEFT_BUTTON:
		{
			if (m_isUpgrading)
			{
				for (auto button : m_buttonUpgradeList)
				{
					if (button->HandleTouchMouse(x, y, pressed))
					{
						ResourcesManager::GetInstance()->GetSound(1)->Play();
						switch (button->m_type)
						{
						case BUTTON_EXIT_UPGRADE:
							m_isUpgrading = false;
							break;
						case BUTTON_BUY_BULLET_UPGRADE:
							m_player->m_coin -= m_totalCoinCostBullet;
							m_totalCoinCost -= m_totalCoinCostBullet;
							m_player->m_numberBullet = m_player->m_numberBullet + m_totalCoinCostBullet / 10 > NUMBER_BULLET_MAX ? NUMBER_BULLET_MAX : m_player->m_numberBullet + m_totalCoinCostBullet / 10;
							m_totalCoinCostBullet = 0;
							break;
						case BUTTON_BUY_ARMOR_UPGRADE:
							m_player->m_coin -= m_totalCoinCostArmor;
							m_totalCoinCost -= m_totalCoinCostArmor;
							m_player->m_armor = m_player->m_armor + m_totalCoinCostArmor / 10 > ARMOR_MAX ? ARMOR_MAX : m_player->m_armor + m_totalCoinCostArmor / 10;
							m_totalCoinCostArmor = 0;
							break;
						case BUTTON_BUY_HP_UPGRADE:
							m_player->m_coin -= m_totalCoinCostHP;
							m_totalCoinCost -= m_totalCoinCostHP;
							m_player->m_health = m_player->m_health + m_totalCoinCostHP / 10 > HP_MAX ? HP_MAX : m_player->m_health + m_totalCoinCostHP / 10;
							m_totalCoinCostHP = 0;
							break;
						case BUTTON_PLUS_ON_BULLET_UPGRADE:
						{
							if (m_totalCoinCost + 50 <= m_player->m_coin)
							{
								m_totalCoinCost += 50;
								m_totalCoinCostBullet += 50;
							}
						}
							break;
						case BUTTON_PLUS_OFF_BULLET_UPGRADE:
						{
							if (m_totalCoinCost - 50 >= 0 && m_totalCoinCostBullet - 50 >= 0)
							{
								m_totalCoinCost -= 50;
								m_totalCoinCostBullet -= 50;
							}
						}
							break;
						case BUTTON_PLUS_ON_ARMOR_UPGRADE:
						{
							if (m_totalCoinCost + 50 <= m_player->m_coin)
							{
								m_totalCoinCost += 50;
								m_totalCoinCostArmor += 50;
							}
						}
							break;
						case BUTTON_PLUS_OFF_ARMOR_UPGRADE:
						{
							if (m_totalCoinCost - 50 >= 0 && m_totalCoinCostArmor - 50 >= 0)
							{
								m_totalCoinCost -= 50;
								m_totalCoinCostArmor -= 50;
							}
						}
							break;
						case BUTTON_PLUS_ON_HP_UPGRADE:
						{
							if (m_totalCoinCost + 50 <= m_player->m_coin)
							{
								m_totalCoinCost += 50;
								m_totalCoinCostHP += 50;
							}
						}
							break;
						case BUTTON_PLUS_OFF_HP_UPGRADE:
						{
							if (m_totalCoinCost - 50 >= 0 && m_totalCoinCostHP - 50 >= 0)
							{
								m_totalCoinCost -= 50;
								m_totalCoinCostHP -= 50;
							}
						}
							break;
						case BUTTON_RESUM:
							m_isUpgrading = false;
							ResourcesManager::GetInstance()->GetSound(3)->Play();
							break;
						case BUTTON_RETRY:
							ResourcesManager::GetInstance()->StopAllSounds();
							ResourcesManager::GetInstance()->GetSound(1)->Play();
							GameStateMachine::GetInstance()->PopState();
							GameStateMachine::GetInstance()->PushState(StateType::STATE_PLAY);
							break;
						case BUTTON_MENU:
							ResourcesManager::GetInstance()->StopAllSounds();
							ResourcesManager::GetInstance()->GetSound(1)->Play();
							GameStateMachine::GetInstance()->PopState();
							break;
						case BUTTON_SETTING:
							GameStateMachine::GetInstance()->PushState(StateType::STATE_SETTING);
							break;
						default:
							break;
						}
						break;
					};
				}
			}
			if (m_player->m_isLost || m_player->m_isVictory)
			{
				for (auto button : m_buttonEndGameList)
				{
					if (button->HandleTouchMouse(x, y, pressed))
					{
						ResourcesManager::GetInstance()->GetSound(1)->Play();
						switch (button->m_type)
						{
						case BUTTON_RETRY:
							ResourcesManager::GetInstance()->StopAllSounds();
							GameStateMachine::GetInstance()->PopState();
							GameStateMachine::GetInstance()->PushState(StateType::STATE_PLAY);
							break;
						case BUTTON_MENU:
							ResourcesManager::GetInstance()->StopAllSounds();
							GameStateMachine::GetInstance()->PopState();
							break;
						default:
							break;
						}
					}
				}
			}
		}
			break;
		case MOUSE_RIGHT_BUTTON:
			break;
		default:
			break;
		}
	}
	else
	{
		switch (key)
		{
		case MOUSE_LEFT_BUTTON:
			break;
		case MOUSE_RIGHT_BUTTON:
			break;
		default:
			break;
		}
	}

}

void GSPlay::OnMouseMove(int x, int y)
{
	for (auto button : m_buttonUpgradeList)
	{
		button->HandleMoveMouse(x, y);
	}
}

void GSPlay::OnMouseScroll(int x, int y, short delta)
{
	if (!m_isUpgrading)
	{
		m_tileSizeByPixel = delta > 0 ? m_tileSizeByPixel + 2 : m_tileSizeByPixel - 2;
		m_tileSizeByPixel = m_tileSizeByPixel < m_minTileSize ? m_minTileSize : m_tileSizeByPixel;
		m_tileSizeByPixel = m_tileSizeByPixel > m_maxTileSize ? m_maxTileSize : m_tileSizeByPixel;
		m_needUpdateDrawSize = true;


		for (auto enemy : m_enemiesList)
		{
			enemy->SetTileSize(m_tileSizeByPixel);
		}
		for (auto bullet : m_bulletList)
		{
			bullet->SetTileSize(m_tileSizeByPixel);
		}
		m_boss->SetTileSizeScroll(m_tileSizeByPixel);
		for (auto obs : m_obsDynamicList)
		{
			obs.sprite->Set2DSize(2 * obs.sizeBox.x * m_tileSizeByPixel, 2 * obs.sizeBox.y * m_tileSizeByPixel);
		}
		for (auto bars : m_barsList)
		{
			if (bars.body->IsEnabled())
			{
				bars.sprite->Set2DSize(m_tileSizeByPixel, bars.height * m_tileSizeByPixel);
			}
		}
		for (auto item : m_itemList)
		{
			if (!item->IsLoot())
			{
				item->Set2DSize(m_tileSizeByPixel, m_tileSizeByPixel);
			}
		}
		m_player->SetTileSize(m_tileSizeByPixel);
	}
}

void GSPlay::LoadMapObstabcles(const std::string& filename)
{
	std::ifstream mapFile(Globals::scenePath + filename);
	if (!mapFile.is_open())
	{
		std::cerr << "Error opening map file: " << Globals::scenePath + filename << "\n";
		return;
	}
	std::cout << "Open map file: " << Globals::scenePath + filename << "\n";
	// Load map
	GLint id;
	mapFile >> id;
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(id);
	m_map = std::make_shared<Sprite2D>(0, model, shader, texture);
	m_map->Set2DPosition(0, 0);

	// Load obs
	GLfloat x, y, w, h, top, down;
	int obsCount;
	mapFile >> m_mapSizeByTile.x >> m_mapSizeByTile.y >> m_minTileSize >> m_maxTileSize >> obsCount;
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2PolygonShape obsShape;
	b2FixtureDef obsFixDef;
	for (int i = 0; i < obsCount; ++i)
	{
		obsBody = nullptr;
		mapFile >> x >> y >> w >> h;
		obsBodyDef.position.Set(x, y);
		obsShape.SetAsBox(w, h);
		obsBody = m_world->CreateBody(&obsBodyDef);
		obsFixDef.shape = &obsShape;
		obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
		obsFixDef.filter.maskBits = 0xFFFF;
		obsBody->CreateFixture(&obsFixDef);
		m_BodyList.push_back(obsBody);
	}

	// Spawn enemies
	std::string skipStr;
	GLint numSpawn, idTypeEnemies;
	mapFile >> skipStr >> numSpawn >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	for (int i = 0; i < numSpawn; i++)
	{
		std::shared_ptr<Enemies> enemies = std::make_shared<Enemies>(i);
		mapFile >> idTypeEnemies;
		enemies->m_boxInfo = SceneManager::GetInstance()->GetBoxEnemy(idTypeEnemies);
		GLfloat left, right;
		mapFile >> x >> y >> left >> right;
		enemies->SetBoundaryMove(left, right);
		enemies->typeEnemeis = idTypeEnemies;
		enemies->CreateEnemyBox(m_world, x, y);
		m_enemiesList.push_back(enemies);
	}

	// Load Obs Dynamic
	mapFile >> skipStr;
	GLint numberObsDynamic;
	mapFile >> numberObsDynamic;
	mapFile >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr >> skipStr;
	for (int i = 0; i < numberObsDynamic; i++)
	{
		mapFile >> id >> x >> y >> w >> h >> down >> top;
		CreateObsDynamic(id, x, y, w, h, top, down);
	}

	// Load bars
	mapFile >> skipStr;
	GLint numberBars;
	mapFile >> numberBars;
	mapFile >> skipStr >> skipStr >> skipStr;
	for (int i = 0; i < numberBars; i++)
	{
		mapFile >> x >> y >> h;
		CreateBars(x, y, h);
	}

	// Load items
	mapFile >> skipStr;
	GLint numberItems;
	mapFile >> numberItems;
	mapFile >> skipStr >> skipStr >> skipStr;
	for (int i = 0; i < numberBars; i++)
	{
		mapFile >> id >> x >> y;
		std::shared_ptr<Item> item = std::make_shared<Item>(m_world.get(), id, x, y);
		m_itemList.push_back(item);
	}
}


void GSPlay::Update2DDrawSize()
{
	// set bullet size
	for (auto& bullet : m_bulletList)
	{
		if (!bullet->m_disabled)
		{
			// set bullet draw size
			bullet->Set2DSizeScroll();
		}
	}

	// set player and map draw size in pixel
	m_player->Set2DSize(m_tileSizeByPixel * 9.0f / 4, m_tileSizeByPixel * 9.0f / 4);
	m_map->Set2DSize(m_tileSizeByPixel * m_mapSizeByTile.x, m_tileSizeByPixel * m_mapSizeByTile.y);

	// set enemy draw size
	for (auto& it : m_enemiesList)
	{
		it->Set2DSizeScroll();
	}
}

void GSPlay::Update2DDrawPosition()
{
	// recalculate camera boundary
	m_cameraPositionBoudaries.z = m_mapSizeByTile.x * m_tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = m_mapSizeByTile.y * m_tileSizeByPixel - (GLfloat)Globals::screenHeight;

	// set camera position to follows player
	auto player_position = m_player->GetPlayerBody()->GetPosition();
	auto newCameraPos = Vector3(player_position.x * m_tileSizeByPixel - (GLfloat)Globals::screenWidth / 2, player_position.y * m_tileSizeByPixel - (GLfloat)Globals::screenHeight / 2, 0.0f);
	newCameraPos.x = newCameraPos.x <= m_cameraPositionBoudaries.x ? m_cameraPositionBoudaries.x : newCameraPos.x;
	newCameraPos.x = newCameraPos.x >= m_cameraPositionBoudaries.z ? m_cameraPositionBoudaries.z : newCameraPos.x;
	newCameraPos.y = newCameraPos.y <= m_cameraPositionBoudaries.y ? m_cameraPositionBoudaries.y : newCameraPos.y;
	newCameraPos.y = newCameraPos.y >= m_cameraPositionBoudaries.w ? m_cameraPositionBoudaries.w : newCameraPos.y;

	m_dynamicCamera->SetPosition(newCameraPos);
	m_dynamicCamera->SetTarget(Vector3(newCameraPos.x, newCameraPos.y, -1.0f));

	// set player draw position
	player_position.x -= 9.0f / 8;
	player_position.y -= 1.5f;
	m_player->Set2DPosition(player_position.x * m_tileSizeByPixel, player_position.y * m_tileSizeByPixel);

	for (auto& bullet : m_bulletList)
	{
		if (!bullet->m_disabled)
		{
			auto pos = bullet->GetBulletBody()->GetPosition();
			bullet->Set2DPosition(pos.x, pos.y);
		}
	}
}

void GSPlay::CreateBullet(TypeBullet typeBullet, FixtureTypes typeFixture, b2Vec2 speed, GLfloat posX, GLfloat posY , GLint currentDirection)
{
	for (auto& bullet : m_bulletList)
	{
		if (bullet->m_disabled)
		{
			bullet->CreateNewBullet(typeBullet, typeFixture, speed, posX, posY, currentDirection);
			break;
		}
	}
}

void GSPlay::CreateObsDynamic(GLint IDTexture, GLfloat posX, GLfloat posY, GLfloat width, GLfloat height, GLfloat top, GLfloat down)
{
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(IDTexture);

	std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->Set2DSize(2 * width * m_tileSizeByPixel, 2 * height * m_tileSizeByPixel);

	// Box
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2PolygonShape obsShape;
	b2FixtureDef obsFixDef;

	obsBodyDef.position.Set(posX, posY);
	obsBodyDef.type = b2_dynamicBody;
	obsShape.SetAsBox(width, height);
	obsBody = m_world->CreateBody(&obsBodyDef);

	obsFixDef.shape = &obsShape;
	obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
	obsFixDef.filter.maskBits = 0xFFFF;
	obsBody->CreateFixture(&obsFixDef);

	ObsDynamic obs;
	obs.sprite = sprite;
	obs.body = obsBody;
	obs.top = top;
	obs.down = down;
	obs.sizeBox = b2Vec2(width, height);
	obs.body->SetLinearVelocity(b2Vec2(0.0f, 0.5f));
	m_obsDynamicList.push_back(obs);
}

void GSPlay::UpdateObsDynamic(GLfloat deltaTime)
{
	for (auto obs : m_obsDynamicList)
	{
		b2Vec2 currentPosition = obs.body->GetPosition();
		if (currentPosition.y <= obs.top)
		{
			obs.body->SetLinearVelocity(b2Vec2(0.0f, 0.5f));
		}
		else if (currentPosition.y >= obs.down)
		{
			obs.body->SetLinearVelocity(b2Vec2(0.0f, -0.5f));
		}

		currentPosition -= b2Vec2(obs.sizeBox.x, obs.sizeBox.y);
		obs.sprite->Set2DPosition(currentPosition.x * m_tileSizeByPixel, currentPosition.y * m_tileSizeByPixel);
	}
}


void GSPlay::CreateBars(GLfloat posX, GLfloat posY, GLfloat height)
{
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(45);

	std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->Set2DSize(m_tileSizeByPixel , height * m_tileSizeByPixel);

	// Box
	b2Body* obsBody;
	b2BodyDef obsBodyDef;
	b2FixtureDef obsFixDef;

	obsBodyDef.position.Set(posX + 0.5f, posY + height / 2.0f);
	obsBodyDef.type = b2_staticBody;
	obsBody = m_world->CreateBody(&obsBodyDef);

	// Fixture
	b2FixtureDef barsFixtureDef;
	b2PolygonShape barsShape;
	b2Fixture* barsFixture;

	barsFixtureDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
	barsFixtureDef.filter.maskBits = 0xFFFF;
	barsShape.SetAsBox(0.5f, height / 2.0f);
	barsFixtureDef.shape = &barsShape;
	barsFixture = obsBody->CreateFixture(&barsFixtureDef);

	ObsDynamic bars;
	bars.sprite = sprite;
	bars.body = obsBody;
	bars.position = Vector2(posX, posY);
	bars.height = height;
	bars.sizeBox = b2Vec2(1.0f, height);
	bars.isLock = (bool*) true;
	m_barsList.push_back(bars);
}

void GSPlay::UpdateBars()
{

	if (m_numberEnemyKill >= 21 && m_player->m_keyItem > 0)
	{
		m_isOnDynamicObs = true;
		m_player->m_keyItem = 0;
	}
	if (m_isOnDynamicObs)
	{
		m_barsList[0].body->SetEnabled(false);
		auto sprite = m_barsList[0].sprite;
		if (sprite->Get2DSize().y >= 0)
		{
			sprite->Set2DSize(sprite->Get2DSize().x, sprite->Get2DSize().y - m_tileSizeByPixel * 0.1);
		}
	}

	for (int i = 1; i < m_barsList.size(); i++)
	{
		GLfloat distance = b2Distance(m_barsList[i].body->GetPosition(), m_player->GetPlayerBody()->GetPosition());
		if (distance < 3.0f && m_barsList[i].isLock && m_isBossDie)
		{
			m_barsList[i].body->SetEnabled(false);
			auto sprite = m_barsList[i].sprite;
			if (sprite->Get2DSize().y >= 0)
			{
				sprite->Set2DSize(sprite->Get2DSize().x, sprite->Get2DSize().y - m_tileSizeByPixel * 0.1);
			}
			else
			{
				m_barsList[i].isLock = (bool*)false;
			}
		}
		else if (distance > 3.0f && !m_barsList[i].isLock)
		{
			m_barsList[i].body->SetEnabled(true);
			auto sprite = m_barsList[i].sprite;
			if (sprite->Get2DSize().y <= m_barsList[i].height * m_tileSizeByPixel)
			{
				sprite->Set2DSize(sprite->Get2DSize().x, sprite->Get2DSize().y + m_tileSizeByPixel * 0.1);
			}
			else
			{
				m_barsList[i].isLock = (bool*)true;
			}
		}
	}

	for (auto bars : m_barsList)
	{
		bars.sprite->Set2DPosition(bars.position.x * m_tileSizeByPixel, bars.position.y * m_tileSizeByPixel);
	}
}


std::shared_ptr<Button> GSPlay::CreateButton(GLint idTexture, GLfloat width, GLfloat height, GLfloat posX, GLfloat posY, ButtonType buttonType)
{
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(idTexture);

	std::shared_ptr<Button> button = std::make_shared<Button>(model, shader, texture, buttonType);
	button->AttachCamera(m_staticCamera);
	button->Set2DSize(width, height);
	button->Set2DPosition(posX, posY);

	return button;
}


void GSPlay::CreateText()
{
	// Coin
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(55);

	std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(20, 20);
	sprite->Set2DPosition(20, 20);
	sprite->Draw();
	m_sprite2DList.push_back(sprite);

	auto font = ResourcesManager::GetInstance()->GetFont(0);
	std::shared_ptr<Text> text = std::make_shared<Text>(0);
	text->AttachCamera(m_staticCamera);
	text->Init(font, std::to_string(m_player->m_coin));
	text->SetTextColor(0, 255, 0);
	text->Set2DScale(0.25f, 0.25f);
	text->Set2DPosition(50, 20.0f);
	//text->Draw();
	m_textList.push_back(text);

	// HP
	texture = ResourcesManager::GetInstance()->GetTexture(48);

	sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->Set2DSize(20, 20);
	sprite->Set2DPosition(150, 20);
	sprite->Draw();
	m_sprite2DList.push_back(sprite);

	std::shared_ptr<Text> text1 = std::make_shared<Text>(1);
	text1->AttachCamera(m_staticCamera);
	text1->Init(font, std::to_string(m_player->m_health));
	text1->SetTextColor(0, 255, 0);
	text1->Set2DScale(0.25f, 0.25f);
	text1->Set2DPosition(180, 20.0f);
	//text1->Draw();
	m_textList.push_back(text1);
	
	// Armor
	texture = ResourcesManager::GetInstance()->GetTexture(56);

	sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(20, 20);
	sprite->Set2DPosition(280, 20);
	sprite->Draw();
	m_sprite2DList.push_back(sprite);

	std::shared_ptr<Text> textArmor = std::make_shared<Text>(2);
	textArmor->AttachCamera(m_staticCamera);
	textArmor->Init(font, std::to_string(m_player->m_armor));
	textArmor->SetTextColor(0, 255, 0);
	textArmor->Set2DScale(0.25f, 0.25f);
	textArmor->Set2DPosition(310, 20.0f);
	textArmor->Draw();
	m_textList.push_back(textArmor);

	// Bullet
	texture = ResourcesManager::GetInstance()->GetTexture(60);

	sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(20, 20);
	sprite->Set2DPosition(410, 20);
	sprite->Draw();
	m_sprite2DList.push_back(sprite);

	std::shared_ptr<Text> bullet = std::make_shared<Text>(3);
	bullet->AttachCamera(m_staticCamera);
	bullet->Init(font, std::to_string(m_player->m_numberBullet));
	bullet->SetTextColor(0, 255, 0);
	bullet->Set2DScale(0.25f, 0.25f);
	bullet->Set2DPosition(440, 20.0f);
	bullet->Draw();
	m_textList.push_back(bullet);

	// Key
	texture = ResourcesManager::GetInstance()->GetTexture(49);

	sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(20, 20);
	sprite->Set2DPosition(540, 20);
	sprite->Draw();
	m_sprite2DList.push_back(sprite);

	std::shared_ptr<Text> key = std::make_shared<Text>(3);
	key->AttachCamera(m_staticCamera);
	key->Init(font, std::to_string(m_player->m_keyItem));
	key->SetTextColor(0, 255, 0);
	key->Set2DScale(0.25f, 0.25f);
	key->Set2DPosition(570, 20.0f);
	key->Draw();
	m_textList.push_back(key);
}

void GSPlay::CreateWhenEndGame()
{
	// Lost
	auto model = ResourcesManager::GetInstance()->GetModel(0);
	auto shader = ResourcesManager::GetInstance()->GetShader(0);
	auto texture = ResourcesManager::GetInstance()->GetTexture(79);

	std::shared_ptr<Sprite2D> sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	sprite->Set2DPosition(0, 0);
	sprite->Draw();
	m_sprite2DEndGameList.push_back(sprite);

	// Victory
	texture = ResourcesManager::GetInstance()->GetTexture(78);
	sprite = std::make_shared<Sprite2D>(0, model, shader, texture);
	sprite->AttachCamera(m_staticCamera);
	sprite->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	sprite->Set2DPosition(0, 0);
	sprite->Draw();
	m_sprite2DEndGameList.push_back(sprite);

	// Button
	Vector2 size = Vector2(110 * 3.0f , 35 * 3.0f );
	GLfloat posX = 25 * 3.0f ;
	GLfloat posY = 154 * 3.0f ;
	m_buttonEndGameList.push_back(CreateButton(73, size.x, size.y, posX, posY, BUTTON_RETRY));

	posX = 184 * 3.0f;
	m_buttonEndGameList.push_back(CreateButton(74, size.x, size.y, posX, posY, BUTTON_MENU));

	// Text
	auto font = ResourcesManager::GetInstance()->GetFont(0);
	m_textTimePlay = std::make_shared<Text>(0);
	m_textTimePlay->AttachCamera(m_staticCamera);
	m_textTimePlay->Init(font, std::to_string((int)m_timePlay));
	m_textTimePlay->SetTextColor(152, 121, 36);
	m_textTimePlay->Set2DScale(0.5f, 0.5f);
	m_textTimePlay->Set2DPosition(170 * 3.0f, 111 * 3.0f);
}

void GSPlay::UpdateGameObjects()
{

}