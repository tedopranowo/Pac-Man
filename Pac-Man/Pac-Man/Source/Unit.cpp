#include "Unit.h"


Unit::Unit()
	: m_position{ -1, -1 }
{
}

Unit::~Unit()
{
	delete m_pGhostAi;
}

void Unit::SetAi(GhostAi * ghostAi)
{
	m_pGhostAi = ghostAi;
}

void Unit::Update(double deltaTime)
{
	if (m_pGhostAi != nullptr)
		m_pGhostAi->Update(deltaTime);
}
