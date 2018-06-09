#include "Tiled.h"

#include "..\..\..\Logging\Source\Logger.h"
#include "..\Xml\tinyxml2.h"
#include "..\GameLogic\GameLogicBase.h"

namespace Tiled
{
	//---------------------------------------------------------------------------
	// This function gets the map width and map height (which is based on the
	// amount of tiles (i.e. a map width of 10 means the map is 10 tiles long).
	// It also gets the width and height of the tiles, which is in pixels.
	//		-filename: the TMX file to get the data from
	//		-return: it returns a const MapData which is a struct of the data.
	//				 We return a const because this data should never change after
	//				 it's intialized.
	//---------------------------------------------------------------------------
	const MapData GetMapData(const char* const filename)
	{
		tinyxml2::XMLDocument map;
		
		// Failed to load the file, default MapData has all data filled with -1
		if (map.LoadFile(filename) != 0)
			return MapData();
		
		tinyxml2::XMLElement *pRoot = map.RootElement();

		const int width			= pRoot->IntAttribute("width");
		const int height		= pRoot->IntAttribute("height");
		const int tileWidth		= pRoot->IntAttribute("tilewidth");
		const int tileHeight	= pRoot->IntAttribute("tileheight");

		return MapData(width, height, tileWidth, tileHeight);
	}

	/*
	Note: there are multiple locations where there are multiple nested for loops, this is O(n^3), but
	we know that the outer loops are very small and a lot of times we break once inside of the inner loops
	*/
	bool Load(const char* filename, GameLogicBase &gameLogic)
	{
		// Load the tmx
		tinyxml2::XMLDocument map;
		std::vector<Tileset> tilesets;
		std::vector<TileType> tileTypes;
		std::vector<std::vector<Tile>> tiles(layerCount);

		if (map.LoadFile(filename) != 0)
		{
			return false;
		}

		

		// Load the tilesets and tile types
		LoadTilesets(tilesets, tileTypes, map);

		// Create the actors
		CreateActorsFromTiledData(tilesets, gameLogic, map);

		// Load the tiles
		LoadTileData(tiles, tilesets, tileTypes, map);
		
		// Create the tiles in the game world
		gameLogic.SetTiles(std::move(tileTypes), std::move(tiles));

		return true;
	}


	//----------------------------------------------------------------------------------------------------------------------------------------------------
	//	This will get the tileset (and tiletypes) data from a TMX fils and store it in the vector arguments
	//		-tilesets:	a vector of tilesets that will store all of the different tilesets' data
	//		-tileTypes:	a vector of the different types of tiles, TileTypes store data regarding the type of tile
	//		-root:		this is the root element of the TMX file to load the data from
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void LoadTilesets(std::vector<Tileset> &tilesets, std::vector<TileType> &tileTypes, const tinyxml2::XMLDocument &map)
	{
		// Load the tileset images
		for (const tinyxml2::XMLElement* element = map.RootElement()->FirstChildElement(); element != 0; element = element->NextSiblingElement())
		{
			// Store the tileset data inside of a struct
			if (*element->Name() == *"tileset")
			{
				std::string imageFilename;
				for (const tinyxml2::XMLElement* childElement = element->FirstChildElement(); childElement != 0; childElement = childElement->NextSiblingElement())
				{
					// Image data should always be the first child element of a tileset, 
					// once we have the tileset, it will allow us to get the x, y, w, h of the tile based on the id

					// Create and store a new tile type
					if (*childElement->Name() == *"tile")
					{
						int id = childElement->IntAttribute("id");
						int size = (int)tilesets.size();

						// Calculate the tile location on the spritesheet
						int srcX = (id % tilesets[size - 1].m_columns) * element->IntAttribute("tilewidth");
						int srcY = (id / tilesets[size - 1].m_columns) * element->IntAttribute("tileheight");

						bool isWalkable{ true };
						int weight{ 0 };

						for (const tinyxml2::XMLElement* properties = childElement->FirstChildElement("properties")->FirstChildElement("property");
							properties != 0; properties = properties->NextSiblingElement())
						{
							// Check if it's the isWalkable property
							if (*properties->Attribute("name") == *"isWalkable")
								isWalkable = properties->BoolAttribute("value");
							else if (*properties->Attribute("name") == *"Weight")
								weight = properties->BoolAttribute("value");
							else
								LOG_WARNING("Tile has an unknown property from Tiled");
						}

						size_t filePos = imageFilename.find_last_of('/');
						std::string file = imageFilename.substr(filePos + 1);
						tileTypes.push_back(TileType(
							id + element->IntAttribute("firstgid"),
							isWalkable,
							file,
							srcX,
							srcY,
							element->IntAttribute("tilewidth"),
							element->IntAttribute("tileheight"),
							weight
						));
						continue;
					}
					// Each tileset should only have 1 image element so we it should be our last check
					else if (*childElement->Name() == *"image")
					{
						// We will need this for the tileTypes
						imageFilename = childElement->Attribute("source");

						tilesets.push_back(Tileset(
							element->IntAttribute("firstgid"),
							element->IntAttribute("tilewidth"),
							element->IntAttribute("tileheight"),
							element->IntAttribute("tilecount"),
							element->IntAttribute("columns"),
							element->Attribute("name"),
							imageFilename
						));
					}
				}
			}
		}
	}

	// TODO: I need to decouple gamelogic from the Tiled namespace.  I need to create a genereic Actor/Object struct that stores the data from TMX
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	//	This will go through a tiled map and create the actors based on the TMX data
	//		-tilesets:	a vector of tilesets used to get data for the actors
	//		-gameLogic:	reference to the game logic layer of the engine.
	//		-map:	the TMX file
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void CreateActorsFromTiledData(const std::vector<Tileset> &tilesets, GameLogicBase &gameLogic, const tinyxml2::XMLDocument &map)
	{
		// Create the actors from the objects
		for (const tinyxml2::XMLElement* element = map.RootElement()->FirstChildElement(); element != 0; element = element->NextSiblingElement())
		{
			if (*element->Name() == *"objectgroup")
			{
				for (const tinyxml2::XMLElement* childElement = element->FirstChildElement(); childElement != 0; childElement = childElement->NextSiblingElement())
				{
					if (*childElement->Name() == *"object")
					{
						// Actors MUST be named in tiled (name should be identical to the xml name)
						std::string actorName = childElement->Attribute("name");
						actorName.append(".xml");
						int gid = childElement->IntAttribute("gid");

						for (unsigned int index{ 0 }; index < tilesets.size(); ++index)
						{
							bool foundTileset{ false };
							int tilesetIndex{ -1 };

							// It's in the last tileset (must check for this first because, we get an error if we do the other check first)
							if (gid >= tilesets[tilesets.size() - 1].m_firstgid)
							{
								foundTileset = true;
								tilesetIndex = (int)tilesets.size() - 1;
							}

							// it's in an early tileset
							else if (gid >= tilesets[index].m_firstgid && gid < tilesets[index + 1].m_firstgid)
							{
								foundTileset = true;
								tilesetIndex = index;
							}

							if (tilesetIndex == -1)
								continue;	// TODO error we should have found a tileset index

							if(foundTileset)
							{
								// the local id (id based on it's position in it's tileset)
								//int id = childElement->IntAttribute("id");

								// gid starts at 1 not 0 for tile object id location in the tileset, so need to decrement by 1 when getting srcX
								int srcY = ((gid - tilesets[tilesetIndex].m_firstgid) / tilesets[tilesetIndex].m_columns) * tilesets[tilesetIndex].m_tileHeight;
								int srcX = ((gid - 1) % tilesets[tilesetIndex].m_columns) * tilesets[tilesetIndex].m_tileWidth;

								size_t pos = tilesets[tilesetIndex].m_imagefile.find_last_of('/');
								std::string docName = tilesets[tilesetIndex].m_imagefile.substr(pos + 1);
								gameLogic.CreateActor(
									actorName,
									childElement->IntAttribute("x"),
									childElement->IntAttribute("y"),
									srcX,
									srcY,
									childElement->IntAttribute("width"),
									childElement->IntAttribute("height"),
									docName.c_str());
								break;
							}
						}
					}
				}
			}
		}
	}

	//----------------------------------------------------------------------------------------------------------------------------------------------------
	//	This will go through the TMX and load all of the tile data into tiles
	//		-tiles:	the vector of tiles that will store the tile data
	//		-tilesets: data for all the tilesets
	//		-tileTypes: data for all of the different tile types
	//		-map:	this it the TMX file data
	//----------------------------------------------------------------------------------------------------------------------------------------------------
	void LoadTileData(std::vector<std::vector<Tile>> &tiles, const std::vector<Tileset> &tilesets, const std::vector<TileType> &tileTypes, const tinyxml2::XMLDocument &map)
	{
		const tinyxml2::XMLElement* root = map.RootElement();

		int mapWidth = root->IntAttribute("width");
		int mapHeight = root->IntAttribute("height");

		std::vector<std::string> mapData(layerCount);

		// Get tiledata (which will be an array of chars)
		for (const tinyxml2::XMLElement* element = root->FirstChildElement(); element != 0; element = element->NextSiblingElement())
		{
			if (*element->Name() == *"layer")
			{
				std::string *pBuffer;
				
				// set the buffer to our collision data string
				if (*element->Attribute("name") == *"CollisionLayer")
				{
					// Collision Layer will always be the last layer
					pBuffer = &mapData[layerCount - 1];
				}

				// set the buffer to our map data string
				else
				{
					// this constant will have to change if I add additional layers
					pBuffer = &mapData[0];
				}

				for (const tinyxml2::XMLElement* childElement = element->FirstChildElement(); childElement != 0; childElement = childElement->NextSiblingElement())
				{
					if (*childElement->Name() == *"data")
					{
						*pBuffer = childElement->GetText();
						break;
					}
				}
			}
		}

		unsigned int layerIndex { 0 };
		for (auto layer : mapData)
		{
			int cellIndex{ 0 };
			std::size_t start{ 0 };
			std::size_t end{ 0 };
			std::size_t length{ 0 };
			std::string subStr;

			while (cellIndex < (mapWidth * mapHeight))
			{
				// Find the position of the next comma
				end = layer.find(',', start + 1);

				// Check if we've reached the end or if we were unable to find a comma
				if (end == std::string::npos)
				{
					// The very last id will NOT have a comma after it, so we must check for \n
					end = layer.find('\n', start + 1);
					if (end == std::string::npos)
						// Okay we've actually reached the end of the file
						break;
				}

				// Get the length to the next comma from our currect position (note: that this is all size_t so A - B == B - A)
				length = layer.find(',', end + 1) - start;

				// Create substring
				// Param1 is Offset from start of string, Param2 is how many characters to read
				subStr = layer.substr(start + 1, length);
				if (subStr != "\n")
				{
					// convert string to int
					int gid = std::stoi(subStr);

					// A gid of 0 means empty tile location
					if (gid == 0)
					{
						start = end;
						++cellIndex;
						tiles[layerIndex].push_back(Tile());
						continue;
					}

					// loop through our tilesets to find the correct tileset
					for (unsigned int tilesetIndex{ 0 }; tilesetIndex < tilesets.size(); ++tilesetIndex)
					{
						bool foundTileset{ false };

						// It's in the last tileset (must check for this first because, we get an error if we do the other check first)
						// tilesets starts at 0 (so we WANT to subtract by 1)
						if (gid >= tilesets[tilesets.size() - 1].m_firstgid)
						{
							foundTileset = true;
							// gid start at 1 (so we do NOT want to subtract 1)
							tilesetIndex = (unsigned int)tilesets.size() - 1;
						}

						// it's in an early tileset
						else if (gid >= tilesets[tilesetIndex].m_firstgid && gid < tilesets[tilesetIndex + 1].m_firstgid)
						{
							foundTileset = true;
						}

						if (foundTileset)
						{
							// convert the tileindex to an x, y coordinate
							int xPos = (cellIndex % mapWidth) * tilesets[tilesetIndex].m_tileWidth;
							int yPos = (cellIndex / mapWidth) * tilesets[tilesetIndex].m_tileHeight;

							// loop through the tileset's tiles to find the correct tile
							for (unsigned int typeIndex{ 0 }; typeIndex < tileTypes.size(); ++typeIndex)
							{
								// We store the individual tiles gid (note that this is not the same is the TMX file)
								// TMX files do not store the gid of the tile.  Each tile has a id based on the tileset
								// so multiple tiles will have the same id.  As of right now I like creating and storing
								// my own global id (gid) for the tileTypes
								if (tileTypes[typeIndex].m_gid == gid)
								{
									tiles[layerIndex].push_back(Tile(
										&tileTypes[typeIndex],
										xPos,
										yPos,
										tileTypes[typeIndex].m_src.w,
										tileTypes[typeIndex].m_src.h));

									break;
								}
							}
							break;
						}
					}
					++cellIndex;
				}
				// Set the start to the end so for our next loop
				start = end;
			}
			++layerIndex;
		}
	}
}