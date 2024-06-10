#include "stdafx.h"

#include "GSMap.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"

extern PlanetType currentMap;

GSMap::~GSMap()
{
}

void GSMap::Init()
{
	m_key = 0;
	m_mouse = 0;
	m_key = 0;
	m_mouse = 0;

	// button play
	auto button = std::make_shared<Button>("btn_play.png", BUTTON_PLAY);
	button->Set2DSize(220, 70);
	button->Set2DPosition(460, 30);
	m_buttonList.push_back(button);

	// button back
	button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	button->Set2DSize(220, 70);
	button->Set2DPosition(710, 30);
	m_buttonList.push_back(button);

	// background
	auto staticCamera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
	m_background = std::make_shared<SpriteAnimation>("GSMap/Space_Stars.png", 4, 0.1f);
	m_background->AttachCamera(staticCamera);
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->Set2DPosition(Globals::screenWidth / 2.0f, Globals::screenHeight / 2.0f);

	if (!m_listSprite2D.empty())
	{
		m_listSprite2D.clear();
	}
	// Planet baren
	auto planet = std::make_shared<Sprite2D>("GSMap/Baren.png");
	planet->Set2DSize(100, 100);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(200, 500);
	m_listSprite2D.push_back(planet);

	// Planet lava
	planet = std::make_shared<Sprite2D>("GSMap/Lava.png");
	planet->Set2DSize(100, 100);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(350, 120);
	m_listSprite2D.push_back(planet);

	// Planet ice
	planet = std::make_shared<Sprite2D>("GSMap/Ice.png");
	planet->Set2DSize(100, 100);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(500, 450);
	m_listSprite2D.push_back(planet);

	// Black hole
	//planet = std::make_shared<Sprite2D>("GSMap/Black_hole.png");
	//planet->Set2DSize(96, 96);
	//planet->AttachCamera(staticCamera);
	//planet->Set2DPosition(670, 170);
	//m_listSprite2D.push_back(planet);

	// Planet terran
	planet = std::make_shared<Sprite2D>("GSMap/Terran.png");
	planet->Set2DSize(100, 100);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(700, 250);
	m_listSprite2D.push_back(planet);

	// Circle
	m_circleChosen = std::make_shared<Sprite2D>("GSMap/Circle.png");
	m_circleChosen->Set2DSize(100, 100);
	m_circleChosen->AttachCamera(staticCamera);
	m_circleChosen->Set2DPosition(m_listSprite2D[currentMap]->Get2DPosition().x, m_listSprite2D[currentMap]->Get2DPosition().y);

}

void GSMap::Update(float deltaTime)
{
	m_background->Update(deltaTime);
}

void GSMap::Draw()
{
	m_background->Draw();
	for (auto it : m_buttonList)
	{
		it->Draw();
	}

	for (auto it : m_listSprite2D)
	{
		it->Draw();
	}
	m_circleChosen->Draw();
}

void GSMap::Pause()
{
}

void GSMap::Resume()
{
}

void GSMap::Exit()
{
}

void GSMap::HandleEvent()
{
}

void GSMap::OnKey(unsigned char key, bool pressed)
{
}

void GSMap::OnMouseClick(int x, int y, unsigned char key, bool pressed)	
{
	for (auto& button : m_buttonList)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_PLAY:
				GameStateMachine::GetInstance()->PopState();
				GameStateMachine::GetInstance()->PushState(StateType::STATE_PLAY);
				return;
			case BUTTON_BACK:
				GameStateMachine::GetInstance()->PopState();
				return;
			default:
				break;
			}
		};
	}

	for (int it = 0; it < m_listSprite2D.size(); it++)
	{
		if (m_listSprite2D[it]->HasTouchMouse(x, y, pressed))
		{
			//GameStateMachine::GetInstance()->PopState();
			currentMap = (PlanetType)(it);
			m_circleChosen->Set2DPosition(m_listSprite2D[it]->Get2DPosition().x, m_listSprite2D[it]->Get2DPosition().y);
			return;
		}
	}
}

void GSMap::OnMouseMove(int x, int y)
{
	for (auto button : m_buttonList)
	{
		button->HandleMoveMouse(x, y);
	}
}

void GSMap::OnMouseScroll(int x, int y, short delta)
{
}
