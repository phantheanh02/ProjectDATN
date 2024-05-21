#include "stdafx.h"
#include "GameStateMachine.h"
#include "GSIntro.h"
#include "GSMenu.h"
#include "GSPlay.h"
#include "GSSetting.h"
#include "GSHighScore.h"
#include "GSCredit.h"
#include "GSMap.h"
#include "GSChooseCharacter.h"

std::shared_ptr<GameStateBase> CreateState(StateType stateType);

void GameStateMachine::Init()
{
	m_pActiveState = nullptr;
	m_pNextState = nullptr;
	m_running = true;
}

void GameStateMachine::CleanUp()
{
	m_pActiveState.reset();
	m_pNextState.reset();
	m_pActiveState = m_pNextState = nullptr;
}

void GameStateMachine::PushState(StateType stateType)
{
	std::shared_ptr<GameStateBase> state = CreateState(stateType);
	if (m_stateStack.empty()) // is first state
	{
		m_stateStack.push(state);
		m_pActiveState = state;
		m_pActiveState->Init();
	}
	else
	{
		m_stateStack.push(state);
		m_pNextState = state;
	}
}

void GameStateMachine::PopState()
{
	m_pActiveState->Exit();
	if (!m_stateStack.empty())
	{
		m_stateStack.pop();
	}
	m_pActiveState = nullptr;
	if (!m_stateStack.empty())
	{
		m_pActiveState = m_stateStack.top();
		m_pActiveState->Resume();
	}
	else
	{
		m_running = false;
	}
}

// perform state change and clean up state stack when program exit
void GameStateMachine::Update()
{
	if (m_pNextState)
	{
		m_pActiveState->Pause();
		m_pActiveState = m_pNextState;
		m_pActiveState->Init();
		m_pNextState = nullptr;
	}
	// destroy states when exit
	if (!m_running)
	{
		while (!m_stateStack.empty())
		{
			auto state = m_stateStack.top();
			state->Exit();
			state = nullptr;
			m_stateStack.pop();
		}
	}
}

void GameStateMachine::Exit()
{
	m_running = false;
}

std::shared_ptr<GameStateBase> GameStateMachine::GetCurrentState()
{
	return m_pActiveState;
}

std::shared_ptr<GameStateBase> CreateState(StateType stateType)
{
	std::shared_ptr<GameStateBase> state = nullptr;
	switch (stateType)
	{
	case StateType::STATE_INTRO:
		state = std::make_shared<GSIntro>(stateType);
		break;
	case StateType::STATE_MENU:
		state = std::make_shared<GSMenu>(stateType);
		break;
	case StateType::STATE_PLAY:
		state = std::make_shared<GSPlay>(stateType);
		break;
	case StateType::STATE_SETTING:
		state = std::make_shared<GSSetting>(stateType);
		break;
	case StateType::STATE_HIGHSCORE:
		state = std::make_shared<GSHighScore>(stateType);
		break;
	case StateType::STATE_CREDIT:
		state = std::make_shared<GSCredit>(stateType);
		break;
	case StateType::STATE_MAP:
		state = std::make_shared<GSMap>(stateType);
		break;
	case StateType::STATE_CHOOSECHARACTER:
		state = std::make_shared<GSChooseCharacter>(stateType);
	case StateType::STATE_INVALID:
		break;
	default:
		break;
	}
	return state;
}