#include "AiFactory.h"

#include "PacmanGame.h"
#include "AiData.h"

void AiFactory::AttachAi(Unit* pUnit, Unit::Type unitType)
{
#if _DEBUG
	pUnit->SetType(unitType);
#endif

	if (unitType != Unit::Type::kPacman)
	{
		GhostAi* pGhostAi = new GhostAi(pUnit, kAiData[static_cast<size_t>(unitType)]);

		pUnit->SetAi(pGhostAi);
	}
}
