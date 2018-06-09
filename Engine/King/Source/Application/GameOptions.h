#pragma once

#include <string>

//--------------------------------------------------------------------------------------
//  This is the data that is required for all games created in this engine.  Every game
//  should have an xml with a single root element with the following attributes
//      "Title"         = title of their game
//      "PreloadFile"   = this is an xml that should contain all of the resources to load
//                        in cache before the game starts.
//      "StartMap"      = the tmx file to load (created via Tiled)
//      
//      TODO: think about changing this name to GameplayInit or ScriptInit or something
//      "StartScript"   = any lua script gameplay initialization needed before we run main game loop
//      "WindowX"       = the x-position of the window
//      "WindowY"       = the y-position of the window
//      "WindowWidth"   = the window width
//      "WindowHeight"  = the window height
//--------------------------------------------------------------------------------------
struct GameOptions
{
    std::string m_title;
    std::string m_preloadXML;
    std::string m_startMap;
    std::string m_startScript;
    std::string m_bgMusic;
    int m_winX;
    int m_winY;
    int m_winWidth;
    int m_winHeight;
};