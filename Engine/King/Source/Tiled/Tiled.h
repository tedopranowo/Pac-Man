#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <cmath>
#include "../Xml/tinyxml2.h"

class GameLogicBase;
struct MapData;

namespace Tiled
{
	constexpr unsigned int layerCount{ 2 };

    // Different types of tiles from TMX file
    struct TileType
    {
        bool m_isWalkable;
        std::string m_filename;
        int m_weight;
        int m_gid;
        SDL_Texture* m_pTexture;
        SDL_Rect m_src;

    public:
        TileType(int gid, bool walkable, std::string filename, int srcX, int srcY, int srcW, int srcH, int weight = INFINITY)
            : m_gid(gid)
            , m_isWalkable(walkable)
            , m_filename(filename)
            , m_weight(weight)
            , m_pTexture(nullptr)
        {
            m_src.x = srcX;
            m_src.y = srcY;
            m_src.w = srcW;
            m_src.h = srcH;
        }

        TileType(TileType&& other)
            : m_gid(std::move(other.m_gid))
            , m_isWalkable(std::move(other.m_isWalkable))
            , m_filename(std::move(other.m_filename))
            , m_weight(std::move(other.m_weight))
            , m_src(std::move(other.m_src))
            , m_pTexture(nullptr)
        {
        }

    };

    // A single tile from the TMX, has a pointer to non-mutable data to save memory
    struct Tile
    {
        const TileType* m_tileData;
        SDL_Rect m_pos;

        bool enabled{ true };

        Tile(const TileType* type = nullptr, int x = 0, int y = 0, int w = 0, int h = 0)
            : m_tileData(type)
        {
            m_pos.x = x;
            m_pos.y = y;
            m_pos.w = w;
            m_pos.h = h;
        }
    };

    // Tiles can come from the same tileset or different tileset
    struct Tileset
    {
        int m_firstgid;
        int m_tileWidth;
        int m_tileHeight;
        int m_tileCount;
        int m_columns;
        std::string m_name;
        std::string m_imagefile;

        Tileset(int gid, int width, int height, int count, int columns, std::string name, std::string image)
            : m_firstgid(gid)
            , m_tileWidth(width)
            , m_tileHeight(height)
            , m_tileCount(count)
            , m_columns(columns)
            , m_name(name)
            , m_imagefile(image)
        {}
    };

    bool Load(const char* filename, GameLogicBase &gameLogic);
	void LoadTilesets(std::vector<Tileset> &tilesets, std::vector<TileType> &tileTypes, const tinyxml2::XMLDocument &map);
	void CreateActorsFromTiledData(const std::vector<Tileset> &tilesets, GameLogicBase &gameLogic, const tinyxml2::XMLDocument &map);
	void LoadTileData(std::vector<std::vector<Tile>> &tiles, const std::vector<Tileset> &tilesets, const std::vector<TileType> &tileTypes, const tinyxml2::XMLDocument &map);
	const MapData GetMapData(const char* const filename);
}