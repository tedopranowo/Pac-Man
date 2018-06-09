#include "AiState.h"

#include "PacmanGame.h"
#include "GhostAi.h"
#include "Unit.h"

#define PRINT_TRANSITION_CHANGE 1

void AiState::Start()
{
#if _DEBUG && PRINT_TRANSITION_CHANGE
	int unitTypeId = static_cast<int>(m_pOwner->GetOwner()->GetType());
	printf("Unit type %d state is changed to %s\n", unitTypeId, m_stateName.c_str());
#endif

	for (auto pTransition : m_transitionList)
	{
		pTransition->Start();
	}
}

void AiState::Update(double deltaTime)
{
	if (!m_isPaused)
	{
		//Loop for every transition
		for (auto pTransition : m_transitionList)
		{
			//Update the transition
			pTransition->Update(deltaTime);

			//If a transition fulfill the condition of changing state
			if (pTransition->CheckChangeStateCondition())
			{
				//Change the state and exit this function
				m_pOwner->ChangeState(pTransition->GetNextState());
				return;
			}
		}
	}
}

void AiState::Exit()
{
	for (auto pTransition : m_transitionList)
	{
		pTransition->Exit();
	}
}

ExactLocationState::ExactLocationState(GhostAi * pOwner, const Vec2 & targetLocation)
	: AiState{ pOwner }
	, m_targetLocation{ targetLocation }
{
}


FunctionalLocationState::FunctionalLocationState(GhostAi * pOwner, LocationFunc targetLocationFunc)
	: AiState{ pOwner }
	, m_targetLocationFunc{ targetLocationFunc }
{
}

Vec2 FunctionalLocationState::GetTargetLocation()
{
	return m_targetLocationFunc(m_pOwner->GetOwner()->GetGame());
}

STTimeLimitExceeded::STTimeLimitExceeded(AiState* pNextState, double timeLimit)
	: AiStateTransition{ pNextState }
	, m_timeLimit{ timeLimit }
{
}

void RandomLocationState::Start()
{
	Map map = m_pOwner->GetOwner()->GetGame()->GetMap();

	//Choose a random location in the map as target location
	int targetX = rand() % map.GetWidth();
	int targetY = rand() % map.GetHeight();

	m_targetLocation.Set(targetX, targetY);
}
