#include "stdafx.h"
#include "GameStateBase.h"

GameStateBase::GameStateBase()
{

}

GameStateBase::GameStateBase(StateType stateType)
{
	m_stateType = stateType;
}

GameStateBase::~GameStateBase()
{

}

StateType GameStateBase::GetStateType()
{
	return m_stateType;
}
