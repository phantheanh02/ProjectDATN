#include "stdafx.h"
#include "GSPlay.h"
#include <cstdlib>
#include <ctime>    

extern int tileSizeByPixel;
extern PlanetType currentMap;
std::shared_ptr<b2World> world;

GSPlay::~GSPlay()
{
	for (auto& it : m_BodyList)
	{
		world->DestroyBody(it);
	}
	world.reset();
	delete m_contactListener;
}

void GSPlay::Init()
{
	if (world)
	{
		world.reset();
	}
	m_key = 0;
	tileSizeByPixel = 30;
	m_totalTime = 0;
	m_isShowPopup = false;
	m_isEndMatch = false;
	m_isWin = false;

	// Box2d
	// create world with gravity
	world = std::make_shared<b2World>(b2Vec2(0, 30));
	m_contactListener = new ContactListener();
	world->SetContactListener(m_contactListener);

	// Player
	m_player = std::make_shared<Player>();

	// Get camera
	m_dynamicCamera = SceneManager::GetInstance()->GetCamera(CameraType::DYNAMIC_CAMERA);
	m_staticCamera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);

	// Map Info
	m_map = SceneManager::GetInstance()->GetMap(currentMap);
	LoadMap();

	// Enemies
	m_boss = std::make_shared<Boss>();
	RandomEnemies();

	// init camera boundaries
	auto size = m_map->GetSizeByTile();
	m_cameraPositionBoudaries.x = 0;
	m_cameraPositionBoudaries.y = 0;
	m_cameraPositionBoudaries.z = size.x * tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = size.y * tileSizeByPixel - (GLfloat)Globals::screenHeight;

	// button back
	CreateButton("Icons/icon_pause.png", 40, 40, 890, 20, BUTTON_PAUSE);

	// HUD and effect
	auto posOrigin = Vector2(40, 40);

	m_loadBullet = std::make_shared<SpriteAnimation>(112, 9, 0.1f);
	m_loadBullet->Set2DSize(50, 50);
	m_loadBullet->Set2DPosition(40, 40);
	m_loadBullet->SetTransparency(0.5);
	m_loadBullet->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));

	// Bullet sprite
	auto sprite = std::make_shared<Sprite2D>(110);
	sprite->Set2DSize(30, 30);
	sprite->Set2DPosition(25, 25);
	sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_spriteHUDList.push_back(sprite);
	// Coin sprite
	sprite = std::make_shared<Sprite2D>("Item/item_coin.png");
	sprite->Set2DSize(30, 30);
	sprite->Set2DPosition(200, 25);
	sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_spriteHUDList.push_back(sprite);
	// Armor sprite
	sprite = std::make_shared<Sprite2D>("Item/item_armor.png");
	sprite->Set2DSize(30, 30);
	sprite->Set2DPosition(375, 25);
	sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_spriteHUDList.push_back(sprite);


	// Text
	auto font = ResourcesManager::GetInstance()->GetFont(0);
	auto staticCamera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
	std::string text = "x" + std::to_string(m_player->GetNumberBullet());

	// number bullet
	m_numberBulletText = std::make_shared<Text>(0);
	m_numberBulletText->AttachCamera(staticCamera);
	m_numberBulletText->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT));
	m_numberBulletText->Init(font, text);
	m_numberBulletText->Set2DPosition(70, 30);
	m_numberBulletText->Set2DScale(0.3f, 0.3f);
	m_numberBulletText->SetTextColor(YELLOW);

	// Number coin
	text = "x" + std::to_string(m_player->GetCoin());
	m_numberCoinText = std::make_shared<Text>(0);
	m_numberCoinText->AttachCamera(staticCamera);
	m_numberCoinText->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT));
	m_numberCoinText->Init(font, text);
	m_numberCoinText->Set2DPosition(235, 30);
	m_numberCoinText->Set2DScale(0.3f, 0.3f);
	m_numberCoinText->SetTextColor(YELLOW);

	// Number armor
	text = "x" + std::to_string(m_player->GetArmor());
	m_numberArmorText = std::make_shared<Text>(0);
	m_numberArmorText->AttachCamera(staticCamera);
	m_numberArmorText->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT));
	m_numberArmorText->Init(font, text);
	m_numberArmorText->Set2DPosition(410, 30);
	m_numberArmorText->Set2DScale(0.3f, 0.3f);
	m_numberArmorText->SetTextColor(YELLOW);

	// Time
	text = "Time: 0s";
	m_totalTimeText = std::make_shared<Text>(0);
	m_totalTimeText->AttachCamera(staticCamera);
	m_totalTimeText->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_TOPRIGHT));
	m_totalTimeText->Init(font, text);
	m_totalTimeText->Set2DPosition(600, 30);
	m_totalTimeText->Set2DScale(0.3f, 0.3f);
	m_totalTimeText->SetTextColor(YELLOW);
}

void GSPlay::Update(float deltaTime)
{
	if (m_isShowPopup)
	{
		UpdatePopup(deltaTime);
		return;
	}

	// Before update, should check win
	CheckWin();

	if (!m_player->IsDie())
	{
		HandleEvent();
		m_totalTime += deltaTime;
	}
	// get refresh rate and set time step
	world->Step(deltaTime, VELOCITY_ITERATION, POSITION_ITERATION);

	Update2DDrawPosition();

	for (auto item : m_itemList)
	{
		item->Update(deltaTime);
	}

	for (auto enemy : m_enemiesList)
	{
		if (enemy->IsActive())
		{
			enemy->Update(deltaTime, m_player->GetBody()->GetPosition());
			if (enemy->IsDie() && !enemy->IsGetCoin())
			{
				m_player->AddCoin(50);
				enemy->SetGetCoinStatus(true);
			}
		}
	}

	m_boss->Update(deltaTime, m_player);
	m_map->Update(deltaTime, m_player->GetBody()->GetPosition());
	
	m_player->Update(deltaTime);
	if (m_player->IsReadyToReset())
	{
		m_isEndMatch = true;
		CreatePopUp();
		//GameStateMachine::GetInstance()->PopState();
	}

	if (m_player->IsLoadingBullet())
	{
		if (m_loadBullet->IsLastFrame())
		{
			m_loadBullet->SetCurrentFrame(0);
			m_player->SetLoadingBullet(0);
		}
		else
		{
			m_loadBullet->Update(deltaTime);
		}
	}

	// Update text
	std::string text = "x" + std::to_string(m_player->GetNumberBullet());
	m_numberBulletText->SetText(text);

	text = "x" + std::to_string(m_player->GetCoin());
	m_numberCoinText->SetText(text);

	text = "x" + std::to_string(m_player->GetArmor());
	m_numberArmorText->SetText(text);

	text = "Time: " + std::to_string((GLint)m_totalTime) + "s";
	m_totalTimeText->SetText(text);
}

void GSPlay::Draw()
{
	m_map->Draw();

	for (auto& button : m_buttonList)
	{
		button->Draw();
	}
	
	for (auto item : m_itemList)
	{
		item->Draw();
	}
	for (auto enemy : m_enemiesList)
	{
		enemy->Draw();
	}
	m_boss->Draw();
	m_player->Draw();

	// HUB
	for (auto sprite : m_spriteHUDList)
	{
		sprite->Draw();
	}
	if (m_player->IsLoadingBullet())
	{
		m_loadBullet->Draw();
	}
	m_numberBulletText->Draw();
	m_numberCoinText->Draw();
	m_numberArmorText->Draw();
	m_totalTimeText->Draw();

	if (m_isShowPopup)
	{
		DrawPopup();
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
}

void GSPlay::HandleEvent()
{
	int processKeyAAndDPressed = 0;
	if (m_key & (1 << 1) && m_keyStack.back() == "A")
	{
		// move left
		processKeyAAndDPressed = 1;
	}
	if (m_key & (1 << 3) && m_keyStack.back() == "D")
	{
		// move right
		processKeyAAndDPressed = 3;
	}

	int event = m_key;
	if (event & (1 << 1) && event & (1 << 3))
	{
		if (processKeyAAndDPressed == 1)
		{
			event ^= 1 << 3; // pop key "D"
		}
		else if (processKeyAAndDPressed == 3)
		{
			event ^= 1 << 1; // pop key "A"
		}
	}

	m_player->HandleEvent(event);
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
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "A") == m_keyStack.end())
				m_keyStack.push_back("A");
			break;
		case KEY_S:
			m_key |= 1 << 2;
			break;
		case KEY_D:
			m_key |= 1 << 3;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "D") == m_keyStack.end())
				m_keyStack.push_back("D");
			break;
		case KEY_SPACE:
			m_key |= 1 << 4;
			break;
		case KEY_J:
			m_key |= 1 << 5;
			break;
		case KEY_ESC:
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
			//m_player->SetCurrentDirectionByPreDirection();
			break;
		case KEY_A:
			m_key ^= 1 << 1;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "A") != m_keyStack.end())
			{
				m_keyStack.erase(std::find(m_keyStack.begin(), m_keyStack.end(), "A"));
			}
			break;
		case KEY_S:
			m_key ^= 1 << 2;
			break;
		case KEY_D:
			m_key ^= 1 << 3;
			if (std::find(m_keyStack.begin(), m_keyStack.end(), "D") != m_keyStack.end())
			{
				m_keyStack.erase(std::find(m_keyStack.begin(), m_keyStack.end(), "D"));
			}
			break;
		case KEY_SPACE:
			m_key ^= 1 << 4;
			break;
		case KEY_J:
			m_key ^= 1 << 5;
			break;
		case KEY_ESC:
			m_isShowPopup = m_isShowPopup ? false : true;
			if (m_isShowPopup)
			{
				CreatePopUp();
			}
			else
			{
				m_buttonList.clear();
				CreateButton("Icons/icon_pause.png", 40, 40, 890, 20, BUTTON_PAUSE);
			}
			break;
		default:
			break;
		}
	}
}

void GSPlay::OnMouseClick(int x, int y, unsigned char key, bool pressed)
{
	bool isResumButtonPressed = false;
	for (auto& button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_BUY_BULLET_UPGRADE:
	
				break;
			case BUTTON_BUY_ARMOR_UPGRADE:
				
				break;
			case BUTTON_BUY_HP_UPGRADE:
				
				break;
			case BUTTON_PLUS_ON_BULLET_UPGRADE:
			
				break;
			case BUTTON_PLUS_OFF_BULLET_UPGRADE:
			
				break;
			case BUTTON_PLUS_ON_ARMOR_UPGRADE:

				break;
			case BUTTON_PLUS_OFF_ARMOR_UPGRADE:
			
				break;
			case BUTTON_PLUS_ON_HP_UPGRADE:

				break;
			case BUTTON_PLUS_OFF_HP_UPGRADE:
			
				break;
			case BUTTON_RESUME:
				m_isShowPopup = false;
				isResumButtonPressed = true;
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
				GameStateMachine::GetInstance()->PopState(); // pop play state
				GameStateMachine::GetInstance()->PopState(); // pop choose map state
				break;
			case BUTTON_SETTING:
				GameStateMachine::GetInstance()->PushState(StateType::STATE_SETTING);
				break;
			case BUTTON_PAUSE:
				CreatePopUp();
				break;
			default:
				break;
			}
		}
	}
	if (isResumButtonPressed)
	{
		m_buttonList.clear();
		CreateButton("Icons/icon_pause.png", 40, 40, 890, 20, BUTTON_PAUSE);
	}
}

void GSPlay::OnMouseMove(int x, int y)
{
	for (auto button : m_buttonList)
	{
		button->HandleMoveMouse(x, y);
	}
}

void GSPlay::OnMouseScroll(int x, int y, short delta)
{
	// Calculate tile size
	auto min = m_map->GetMinTileSize();
	auto max = m_map->GetMaxTileSize();
	tileSizeByPixel = delta > 0 ? tileSizeByPixel + 2 : tileSizeByPixel - 2;
	tileSizeByPixel = tileSizeByPixel < min ? min : tileSizeByPixel;
	tileSizeByPixel = tileSizeByPixel > max ? max : tileSizeByPixel;

	// Calculate size
	m_map->OnMouseScroll();
	m_player->ReCalculateWhenScroll();
	for (auto enemy : m_enemiesList)
	{
		if (enemy->IsActive())
		{
			enemy->OnMouseScroll();
		}
	}
}

void GSPlay::Update2DDrawPosition()
{
	// recalculate camera boundary
	auto size = m_map->GetSizeByTile();
	m_cameraPositionBoudaries.z = size.x * tileSizeByPixel - (GLfloat)Globals::screenWidth;
	m_cameraPositionBoudaries.w = size.y * tileSizeByPixel - (GLfloat)Globals::screenHeight;

	// set camera position to follows m_player
	auto m_player_position = m_player->GetBody()->GetPosition();
	auto newCameraPos = Vector3(m_player_position.x * tileSizeByPixel - (GLfloat)Globals::screenWidth / 2, m_player_position.y * tileSizeByPixel - (GLfloat)Globals::screenHeight / 2, 0.0f);
	newCameraPos.x = newCameraPos.x <= m_cameraPositionBoudaries.x ? m_cameraPositionBoudaries.x : newCameraPos.x;
	newCameraPos.x = newCameraPos.x >= m_cameraPositionBoudaries.z ? m_cameraPositionBoudaries.z : newCameraPos.x;
	newCameraPos.y = newCameraPos.y <= m_cameraPositionBoudaries.y ? m_cameraPositionBoudaries.y : newCameraPos.y;
	newCameraPos.y = newCameraPos.y >= m_cameraPositionBoudaries.w ? m_cameraPositionBoudaries.w : newCameraPos.y;

	m_dynamicCamera->SetPosition(newCameraPos);
	m_dynamicCamera->SetTarget(Vector3(newCameraPos.x, newCameraPos.y, -1.0f));

	// set m_player draw position
	m_player_position.x -= 9.0f / 8;
	m_player_position.y -= 1.5f;
	m_player->Set2DPositionByTile(m_player_position.x, m_player_position.y );


}

void GSPlay::LoadMap()
{
	auto data = m_map->GetData();

	// Load obs	
	auto planeList = data->tiles;
	for (const auto& tile : planeList)
	{
		b2BodyDef bodyDef;
		b2FixtureDef obsFixDef;
		//bodyDef.type = b2_staticBody;
		bodyDef.position.Set(tile.x + 0.5, tile.y + 0.5);
		b2Body* body = world->CreateBody(&bodyDef);

		b2PolygonShape shape;
		shape.SetAsBox(0.5, 0.5);
		obsFixDef.filter.categoryBits = FixtureTypes::FIXTURE_GROUND;
		obsFixDef.filter.maskBits = 0xFFFF;
		obsFixDef.shape = &shape;
		body->CreateFixture(&obsFixDef);
		m_BodyList.push_back(body);
	}

	// Load item
	auto itemList = data->items;
	for (const auto& it : itemList)
	{
		auto item = std::make_shared<Item>(it.type, it.posX, it.posY);
		m_itemList.push_back(item);
	}
}

void GSPlay::CreatePopUp()
{
	m_isShowPopup = true;
	m_listPopupSprite.clear();
	m_buttonList.clear();

	// add new button
	Vector2 size = Vector2(137, 64);
	GLfloat posX = 430;
	GLfloat posY = 233;

	// Background
	auto sprite = std::make_shared<Sprite2D>("Background/bg_black.png");
	sprite->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	sprite->Set2DPosition(0, 0);
	sprite->SetTransparency(0.5f);
	sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
	m_listPopupSprite.push_back(sprite);

	if (m_isEndMatch)
	{
		if (m_isWin)
		{
			if (m_totalTime < TIME_END_3S)
			{
				sprite = std::make_shared<Sprite2D>("Background/bg_victory3star.png");
			}
			else if (m_totalTime < TIME_END_2S)
			{
				sprite = std::make_shared<Sprite2D>("Background/bg_victory2star.png");
			}
			else
			{
				sprite = std::make_shared<Sprite2D>("Background/bg_victory1star.png");
			}
		}
		else
		{
			sprite = std::make_shared<Sprite2D>("Background/bg_lost.png");
		}

		sprite->Set2DSize(640, 480);
		sprite->Set2DPosition(480, 360);
		sprite->SetModel(ResourcesManager::GetInstance()->GetModel(ModelType::R_RETANGLE_CENTER));
		sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
		m_listPopupSprite.push_back(sprite);

		CreateButton("Button/btn_retry.png", 220, 70, 190, 462, BUTTON_RETRY);
		CreateButton("Button/btn_menu.png", 220, 70, 550, 462, BUTTON_MENU);
	}
	else
	{
		sprite = std::make_shared<Sprite2D>("Background/bg_popup.png");
		sprite->Set2DSize(850, 430);
		sprite->Set2DPosition(55, 145);
		sprite->AttachCamera(SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA));
		m_listPopupSprite.push_back(sprite);

		CreateButton("Button/btn_resume.png", 193, 61, 679, 225, BUTTON_RESUME);
		CreateButton("Button/btn_retry.png", 193, 61, 679, 304, BUTTON_RETRY);
		CreateButton("Button/btn_menu.png", 193, 61, 679, 383, BUTTON_MENU);
		CreateButton("Button/btn_setting.png", 193, 61, 679, 462, BUTTON_SETTING);

		CreateButton("Button/btn_buy.png", 137, 63, 431, 234, BUTTON_BUY_BULLET_UPGRADE);
		CreateButton("Button/btn_buy.png", 137, 63, 431, 351, BUTTON_BUY_ARMOR_UPGRADE);
		CreateButton("Button/btn_buy.png", 137, 63, 431, 466, BUTTON_BUY_HP_UPGRADE);

		CreateButton("Button/btn_plus_on.png", 19, 19, 380, 255, BUTTON_PLUS_ON_BULLET_UPGRADE);
		CreateButton("Button/btn_plus_on.png", 19, 19, 380, 373, BUTTON_PLUS_ON_ARMOR_UPGRADE);
		CreateButton("Button/btn_plus_on.png", 19, 19, 380, 487, BUTTON_PLUS_ON_HP_UPGRADE);
		CreateButton("Button/btn_plus_off.png", 19, 8, 257, 260, BUTTON_PLUS_OFF_BULLET_UPGRADE);
		CreateButton("Button/btn_plus_off.png", 19, 8, 257, 378, BUTTON_PLUS_OFF_ARMOR_UPGRADE);
		CreateButton("Button/btn_plus_off.png", 19, 8, 257, 487, BUTTON_PLUS_OFF_HP_UPGRADE);
	}
}

void GSPlay::DrawPopup()
{
	for (auto sprite : m_listPopupSprite)
	{
		sprite->Draw();
	}
	for (auto& button : m_buttonList)
	{
		button->Draw();
	}


}

void GSPlay::UpdatePopup(float deltaTime)
{
}

void GSPlay::CreateButton(const char* filename, GLfloat width, GLfloat height, GLfloat posX, GLfloat posY, ButtonType buttonType)
{
	auto button = std::make_shared<Button>(filename, buttonType);
	button->Set2DSize(width, height);
	button->Set2DPosition(posX, posY);
	m_buttonList.push_back(button);
}

void GSPlay::CheckWin()
{
	for (auto enemy : m_enemiesList)
	{
		if (enemy->IsActive())
		{
			return;
		}
	}

	m_isEndMatch = true;
	m_isWin = true;
	CreatePopUp();
}

void GSPlay::RandomEnemies()
{
	auto data = m_map->GetData();
	auto spawnPosition = m_map->GetData()->enemiesSpawn.spawnPosition;
	auto enemiesRatio = m_map->GetData()->enemiesSpawn.enemiesTypeRatio;

	srand(static_cast<unsigned int>(time(0)));

	for (int i = 0; i < spawnPosition.size(); ++i) 
	{
		double randValue = static_cast<double>(rand()) / RAND_MAX;
		double cumulativeProbability = 0.0;

		for (const auto& type : enemiesRatio) 
		{
			cumulativeProbability += type.second;
			if (randValue <= cumulativeProbability) 
			{
				auto enemy = SceneManager::GetInstance()->GetEnemy(type.first);
				enemy->Init(spawnPosition[i].x, spawnPosition[i].y);
				m_enemiesList.push_back(enemy);
				break;
			}
		}
	}
}
