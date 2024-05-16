#include "stdafx.h"

#include "GSMap.h"
#include "ResourcesManager.h"
#include "SceneManager.h"
#include "GameStateMachine.h"

GSMap::~GSMap()
{
}

void GSMap::Init()
{
	m_key = 0;
	m_mouse = 0;
	m_key = 0;
	m_mouse = 0;
	m_typeMap = 0;

	// button play
	auto button = std::make_shared<Button>("btn_play.png", BUTTON_PLAY);
	button->Set2DSize(110, 35);
	button->Set2DPosition(650, 40);
	m_listButton.push_back(button);

	// button back
	button = std::make_shared<Button>("btn_back.png", BUTTON_BACK);
	button->Set2DSize(110, 35);
	button->Set2DPosition(800, 40);
	m_listButton.push_back(button);

	// background
	auto staticCamera = SceneManager::GetInstance()->GetCamera(CameraType::STATIC_CAMERA);
	m_background = std::make_shared<SpriteAnimation>("GSMap/Space_Stars.png", 1, 4, 0.1f);
	m_background->AttachCamera(staticCamera);
	m_background->Set2DSize(Globals::screenWidth, Globals::screenHeight);
	m_background->Set2DPosition(0, 0);

	// Planet baren
	auto planet = std::make_shared<Sprite2D>("GSMap/Baren.png");
	planet->Set2DSize(96, 96);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(180, 540);
	m_listSprite2D.push_back(planet);

	// Planet ice
	planet = std::make_shared<Sprite2D>("GSMap/Ice.png");
	planet->Set2DSize(96, 96);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(450, 420);
	m_listSprite2D.push_back(planet);

	// Planet lava
	planet = std::make_shared<Sprite2D>("GSMap/Lava.png");
	planet->Set2DSize(96, 96);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(310, 130);
	m_listSprite2D.push_back(planet);

	// Black hole
	planet = std::make_shared<Sprite2D>("GSMap/Black_hole.png");
	planet->Set2DSize(96, 96);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(670, 170);
	m_listSprite2D.push_back(planet);

	// Planet terran
	planet = std::make_shared<Sprite2D>("GSMap/Terran.png");
	planet->Set2DSize(96, 96);
	planet->AttachCamera(staticCamera);
	planet->Set2DPosition(730, 460);
	m_listSprite2D.push_back(planet);

	// Circle
	m_circleChosen = std::make_shared<Sprite2D>("GSMap/Circle.png");
	m_circleChosen->Set2DSize(96, 96);
	m_circleChosen->Set2DPosition(180, 540);
	m_circleChosen->AttachCamera(staticCamera);
}

void GSMap::Update(float deltaTime)
{
	m_background->Update(deltaTime);
}

void GSMap::Draw()
{
	m_background->Draw();
	for (auto it : m_listButton)
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
	for (auto& button : m_listButton)
	{
		if (button->HandleTouchMouse(x, y, pressed))
		{
			ResourcesManager::GetInstance()->GetSound(1)->Play();
			switch (button->m_type)
			{
			case BUTTON_PLAY:
				SceneManager::GetInstance()->SetCurrentMap(m_typeMap);
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
			m_typeMap = it;
			m_circleChosen->Set2DPosition(m_listSprite2D[it]->Get2DPosition().x, m_listSprite2D[it]->Get2DPosition().y);
			return;
		}
	}
}

void GSMap::OnMouseMove(int x, int y)
{
}

void GSMap::OnMouseScroll(int x, int y, short delta)
{
}
