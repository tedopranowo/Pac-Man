#include "Map.h"

#include <fstream>

Map::Map()
	: m_ghostHomePos{ -1, -1 }
{
}


Map::~Map()
{
}

void Map::Read(const std::string & filePath)
{
	std::vector<TileType> row;
	char ch;

	//Open file path
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		printf("Failed to open map");
	}

	//Reading the map file
	while (file.get(ch))
	{
		//If it is end of file, push the row into the tile chunk
		if (ch == '\n')
		{
			m_tiles.push_back(row);
			row.clear();
		}
		//If it is not the end of file, push the data into the row
		else
		{
			//Convert from ascii to TileType
			TileType tile = static_cast<TileType>(ch - '0');

			//if it is a home, set it as ghost home
			if (tile == TileType::kGhostHome)
				m_ghostHomePos = Vec2(row.size(), m_tiles.size());

			row.push_back(tile);
		}
	}

	// Close the file
	file.close();
}

bool Map::IsValid(const Vec2 & position) const
{
	return IsValid(static_cast<int>(position.GetX()), static_cast<int>(position.GetY()));
}

bool Map::IsValid(const size_t x, const size_t y) const
{
	if (y >= 0 && y < m_tiles.size() && x >= 0 && x < m_tiles[y].size())
		return true;

	return false;
}

bool Map::IsWalkable(const Vec2 & position) const
{
	return IsWalkable(static_cast<int>(position.GetX()), static_cast<int>(position.GetY()));
}

bool Map::IsWalkable(const size_t x, const size_t y) const
{
	if (IsValid(x, y))
		return static_cast<int>(m_tiles[y][x]) <= kWalkableMaxIndex;

	return false;
}

bool Map::IsIntersection(const Vec2 & position) const
{
	return IsIntersection(static_cast<int>(position.GetX()), static_cast<int>(position.GetY()));
}

bool Map::IsIntersection(const size_t x, const size_t y) const
{
	int walkableAdjacentCount = 0;
	walkableAdjacentCount += IsWalkable(x - 1, y) || IsGate(x - 1, y);
	walkableAdjacentCount += IsWalkable(x + 1, y) || IsGate(x + 1, y);
	walkableAdjacentCount += IsWalkable(x, y - 1) || IsGate(x, y - 1);
	walkableAdjacentCount += IsWalkable(x, y + 1) || IsGate(x, y + 1);

	if (walkableAdjacentCount > 2)
		return true;
	
	return false;
}

bool Map::IsGate(const Vec2 & position) const
{
	return IsGate(static_cast<int>(position.GetX()), static_cast<int>(position.GetY()));
}

bool Map::IsGate(const size_t x, const size_t y) const
{
	if (IsValid(x, y))
		return m_tiles[y][x] == TileType::kGate;

	return false;
}
