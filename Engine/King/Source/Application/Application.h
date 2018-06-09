#pragma once

#include <memory>
#include <vector>
#include <lua.hpp>

#include "GameOptions.h"
#include "..\Events\QuitApplicationEvent.h"

class OpSys;
class ResourceCache;
class EventManager;
struct SDL_Window;
struct SDL_Renderer;
class GameLogicBase;
class LuaManager;
class InputSystem;
class GameTimer;
class AudioSystem;

#define CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, descriptor, identifier, function) luaL_Reg descriptor; descriptor.name = identifier; descriptor.func = function; \
pApp->AddCustomCFunction(descriptor);

class Application
{
private:
    constexpr static double m_kEventTime{ 10000 }; // The time allotted for the eventSystem to trigger events each frame
    constexpr static int m_kMemoryRequired{ 50 };

    OpSys* m_pOperatingSystem;
    ResourceCache* m_pResourceCache;
    EventManager* m_pEventManager;
	SDL_Window* m_pWin;
	SDL_Renderer* m_pRen;
    GameLogicBase* m_pGame;
    LuaManager* m_pLuaManager;
    InputSystem* m_pInputSystem;
	GameTimer* m_pGameTimer;
    EventListenerDelegate m_quitAppDelegate;

    AudioSystem *m_pAudioSystem;

	std::vector<luaL_Reg> m_customCFunctions;

    bool m_running{ true };

public:
    Application();
    ~Application() {};

    GameOptions m_gameOptions;

    void Run();
    void Cleanup();

    // Accessors
    InputSystem* const GetInputSystem() { return m_pInputSystem; }
    ResourceCache* const GetResourceCache() { return m_pResourceCache; }
	LuaManager* const GetLuaManager() { return m_pLuaManager;  }

    void PlayBgMusic(bool play);

	// Lua
	void AddCustomCFunction(const luaL_Reg& customFunctionDesc);
    
private:
    const bool Initialize();
    const bool InitializeLog();
    const bool InitializeOS();
    const bool CreateEventManager();
    const bool CreateResourceCache();
    const bool SetGameOptions();
    const bool InitializeSDL();
    const bool CreateSDLWindow();
    const bool CreateSDLRenderer();
    const bool InitializeSDLMixer();
    const bool InitializeSDLText();
    const bool CreateGameLogicAndGameView();
    const bool CreateLuaManager();

    bool PostInitialization();
    void Play();
    void QuitAppFunc(EventDataPtr eventData);
};