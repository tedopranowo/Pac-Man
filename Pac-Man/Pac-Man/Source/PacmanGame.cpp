#include "PacmanGame.h"

#include <time.h>

#include "AiFactory.h"

PacmanGame::PacmanGame()
{
	Initialize();
}

void PacmanGame::Initialize()
{
	srand(static_cast<unsigned int>(time(nullptr)));
}

void PacmanGame::RegisterUnit(int id, Unit::Type type, const Vec2& position)
{
	int index = static_cast<int>(type);

	// Check if there exist a unit with the same type
	if (!m_units[index].IsNull())
	{
		printf("Warning: there exist a unit with the same type");
		return;
	}

	// Set the unit game pointer
	m_units[index].SetGame(this);

	// Set the unit position
	m_units[index].SetPosition(position);

	// Create the ai for the unit
	AiFactory::AttachAi(&m_units[index], type);

	// Add the id of the unit to the map
	m_idToIndex[id] = static_cast<int>(type);
}
