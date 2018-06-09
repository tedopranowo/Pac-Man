// Application.cpp

#include "Application.h"

#include <iostream>

#include "..\Xml\tinyxml2.h"
#include "..\EventSystem\EventManager.h"
#include "..\ResourceManager\ResourceCache.h"
#include "..\ResourceManager\DefaultResourceLoader.h"
#include "..\ResourceManager\GraphicResourceLoader.h"
#include "..\ResourceManager\ResourceHandle.h"
#include "..\GameLogic\GameLogicBase.h"
#include "..\GameView\GameViewBase.h"
#include "..\GameTimer\GameTimer.h"
#include "..\InputSystem\InputSystem.h"
#include "..\AudioSystem\AudioSystem.h"
#include "..\LuaManager\LuaManager.h"
#include "..\..\..\OS\Source\OpSys.h"
#include "..\..\..\Logging\Source\Logger.h"

// SDL Headers
#include "..\..\SDL2-2.0.5\include\SDL.h"
#include "..\..\SDL2_image-2.0.1\include\SDL_image.h"
#include "..\..\SDL2_ttf\include\SDL_ttf.h"
#include "..\..\SDL2_mixer\include\SDL_mixer.h"

//--------------------------------------------------------------------------------------
//  This is the Application layer of the engine.  This initializes the different systems
//  and contains the main loop and game logic.  The key files that are required for the
//  application to initialize correctly are Assets\XML\Test.xml, Assets\GameOptions.xml,
//  and Preload.xml.
//      -m_pOperatingSystem:    operating system
//      -m_pResourceCache:      this is the resource cache that holds all the resources
//      -m_pEventManager:       the event manager (global/singleton)
//      -m_pWin:    this is the the game window
//      -m_pRen:    this is the renderer for the game
//      -m_pGame:   this is the game logic
//      -m_pLuaManager:     this handles communication between Lua and C++
//      -m_pInputSystem:    this handles the input for the game
//      -m_quitAppDelegate: this is the delegate that quits the application
//      -m_running: boolean that controls the main game loop.  When true game runs,
//                  if false the the game loop will stop looping and the application will end   
//      -m_kEventTime:  The time allotted for the eventSystem to trigger events each frame
//
//      TODO:   maybe have a memory attribute for the GameOptions.xml and actually do stuff with
//              this variable
//      -m_kMemoryRequired: the memory required to play the game
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
//  Constructor
//--------------------------------------------------------------------------------------
Application::Application()
    : m_pOperatingSystem(nullptr)
    , m_pResourceCache(nullptr)
    , m_pEventManager(nullptr)
    , m_pWin(nullptr)
    , m_pRen(nullptr)
    , m_pGame(nullptr)
    , m_pLuaManager(nullptr)
    , m_pInputSystem(nullptr)
    , m_pGameTimer(nullptr)
    , m_running(true)
{
}

//--------------------------------------------------------------------------------------
//  This should be called in main.cpp by all games using this engine.
//--------------------------------------------------------------------------------------
void Application::Run()
{
    // Initialize the engine and game
    bool success = Initialize();

    // Game initialized successfully
    if (success == true)
    {
        if (PostInitialization())
        {
            // run the game
            LOG_INIT("Game Initialized!");
            Play();
        }

        // Post Initialization failed
        else
        {
            LOG_ERROR("Post Initialization Failed.");
        }
    }

    // Initialization failed
    else
    {
        LOG_ERROR("Failed to Initialize Game.");
    }

    // Shutdown the engine
	Cleanup();
}

void Application::AddCustomCFunction(const luaL_Reg & customFunctionDesc)
{
	m_customCFunctions.push_back(customFunctionDesc);
}

//--------------------------------------------------------------------------------------
//  This will initialize each system 1 by 1.  If any system fails to load we stop and
//  shutdown the engine.
//      -return: false, if any system failed to load.  True, if all systems loaded correctly
//--------------------------------------------------------------------------------------
const bool Application::Initialize()
{
    if (!InitializeLog())
        return false;
    if (!InitializeOS())
        return false;
    if (!CreateEventManager())
        return false;
    if (!CreateResourceCache())
        return false;
    if (!SetGameOptions())
        return false;
    if (!InitializeSDL())
        return false;
    if (!CreateSDLWindow())
        return false;
    if (!CreateSDLRenderer())
        return false;
    if (!InitializeSDLMixer())
        return false;
    if (!InitializeSDLText())
        return false;
    if (!CreateGameLogicAndGameView())
        return false;
    if (!CreateLuaManager())
        return false;

    return true;
}

//--------------------------------------------------------------------------------------
//  Initialization of all the engine systems
//--------------------------------------------------------------------------------------

// Create the logging system
const bool Application::InitializeLog()
{
    // Initialize the logging system
    bool success = Logger::Initialize("MyEngine.log");
    if (success)
    {
        LOG_INIT("Logger");
        return true;
    }
    else
    {
        LOG_ERROR("Logger failed to open.");
        return false;
    }
}

// Create operating system
const bool Application::InitializeOS()
{
    m_pOperatingSystem = OpSys::Create();
    return m_pOperatingSystem->CheckFreeDiskSpace(1024 << 10);
}

// Create our resource cache
const bool Application::CreateResourceCache()
{
    m_pResourceCache = new ResourceCache(m_kMemoryRequired);
    if (m_pResourceCache)
    {
        m_pResourceCache->Init();
        LOG_INIT("Resource Cache Created");
        return true;
    }
    else
    {
        LOG_ERROR("Failed to create Resource Cache");
        return false;
    }
}

// Create a global event manager
const bool Application::CreateEventManager()
{
    m_pEventManager = new EventManager("Main Event Manager", true);
    if (m_pEventManager)
    {
         LOG_INIT("Application Event Manager Created");
         return true;
    }
    else
    {
        LOG_ERROR("Failed to Create Event Manager");
        return false;
    }
}

//--------------------------------------------------------------------------------------
//  This function initializes the required starting game data for all games.
//  Every game created in this engine must have an xml file "GameOptions.xml".
//  (see GameOptions.h for further information regarding the data required)
//--------------------------------------------------------------------------------------
const bool Application::SetGameOptions()
{
    // game options data
    tinyxml2::XMLDocument gameOptions;

    // load the xml file
    if (gameOptions.LoadFile("Assets/XML/GameOptions.xml") == 0)
    {
        // load all of the required starting data
        const tinyxml2::XMLElement* root = gameOptions.RootElement();

        m_gameOptions.m_title       = root->Attribute("Title");
        m_gameOptions.m_preloadXML  = root->Attribute("PreloadFile");
        m_gameOptions.m_startMap    = root->Attribute("StartMap");
        m_gameOptions.m_startScript = root->Attribute("StartScript");
        m_gameOptions.m_bgMusic     = root->Attribute("BgMusic");
        m_gameOptions.m_winX        = root->IntAttribute("WindowX");
        m_gameOptions.m_winY        = root->IntAttribute("WindowY");
        m_gameOptions.m_winWidth    = root->IntAttribute("WindowWidth");
        m_gameOptions.m_winHeight   = root->IntAttribute("WindowHeight");

        LOG_INIT("Game Options Set");
        return true;
    }

    // failed to find the GameOptions xml
    else
    {
        LOG_ERROR("Unable to load GameOptions.xml");
        gameOptions.PrintError();
        return false;
    }
}

//--------------------------------------------------------------------------------------
//  Initialize SDL
//--------------------------------------------------------------------------------------
const bool Application::InitializeSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        LOG_INIT("SDL");
        return true;
    }
    else
    {
        LOG_ERROR(SDL_GetError());
        return false;
    }
}

const bool Application::CreateSDLWindow()
{
    m_pWin = SDL_CreateWindow(
        m_gameOptions.m_title.c_str(),
        m_gameOptions.m_winX,
        m_gameOptions.m_winY,
        m_gameOptions.m_winWidth,
        m_gameOptions.m_winHeight,
        SDL_WINDOW_OPENGL);

    if (m_pWin)
    {
        LOG_INIT("SDL window created");
        return true;
    }
    else
    {
        LOG_ERROR(SDL_GetError()); 
        return false;
    }

}

const bool Application::CreateSDLRenderer()
{
    m_pRen = SDL_CreateRenderer(m_pWin, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (m_pRen)
    {
        LOG_INIT("SDL renderer created");
        return true;
    }
    else
    {
        LOG_ERROR(SDL_GetError()); 
        return false;
    }

}

const bool Application::InitializeSDLText()
{
    if (TTF_Init() == 0)
    {
        LOG_INIT("SDL true-type font");
        return true;
    }
    else
    {
        LOG_ERROR(SDL_GetError());
        return false;
    }
}

const bool Application::InitializeSDLMixer()
{
    // Mix_Init() returns flags successfully initialized, or 0 on failure.
    if (Mix_Init(MIX_INIT_OGG) != 0)
    {
        LOG_INIT("SDL mixer");
        Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024);
        
        // Create the audio system
        m_pAudioSystem = new AudioSystem(m_gameOptions.m_bgMusic.c_str());
        return true;
    }
    else
    {
        LOG_ERROR(SDL_GetError());
        return false;
    }
}

//--------------------------------------------------------------------------------------
//  Create the Game Logic and create/attach a Game View.  The Application
//  does not hold onto or store the Game View.  The Game View is accessed through
//  the Game Logic.
//--------------------------------------------------------------------------------------
const bool Application::CreateGameLogicAndGameView()
{
    // Create the Game Logic
    m_pGame = new GameLogicBase(this);

    // Created Game Logic
    if (m_pGame)
    {
        // Create and attach the starting Game View
        m_pGame->AttachView(std::make_shared<GameViewBase>(this, m_pRen));
        LOG_INIT("Created Game Logic and Game View");
        return true;
    }

    // Failed to create Game Logic
    else
    {
        LOG_ERROR("Failed to Create Game Logic");
        return false;
    }
}

//--------------------------------------------------------------------------------------
//  Create the Lua Manager
//--------------------------------------------------------------------------------------
const bool Application::CreateLuaManager()
{
    m_pLuaManager = new LuaManager(m_pGame, m_pResourceCache);
    return (m_pLuaManager != nullptr);
}

//--------------------------------------------------------------------------------------
//  Post-init setup.  This loads the initial resources needed, creates input system,
//  creates the lua state to run gameplay scripts, and loads the initial map.
//--------------------------------------------------------------------------------------
bool Application::PostInitialization()
{
    // Load the preload resources (the resources that the start of the game uses)
    assert(m_gameOptions.m_preloadXML.empty() == false);
    m_pResourceCache->LoadMultipleResourcesFromXml(m_gameOptions.m_preloadXML.c_str());
    
    // We can now quit the game by pressing 'escape'
    MAKE_AND_BIND_DELEGATE(m_quitAppDelegate, this, &Application::QuitAppFunc, QuitApplicationEvent::s_kEventType);
    
    // TODO: move this into its own function??
    // Create our Input System
    m_pInputSystem = new InputSystem(this);
    if (m_pInputSystem == false)
    {
        return false;
    }

	m_pGameTimer = new GameTimer();

	// Add the C++ functions to lua
	m_pLuaManager->AddCustomCFunctions(std::move(m_customCFunctions));
	m_pLuaManager->ExposeCustomCFunctionsToLua();

    // TODO: I think CreateInitialLuaState() and LoadMap() should be put into it's own function
    // this would allow new levels to be loaded

    // Setup the start of the game
    m_pGame->CreateInitialLuaState();

    // Load the startup Tiled map
    if (m_pGame->LoadMap(m_gameOptions.m_startMap.c_str()) == false)
    {
        LOG_ERROR("Failed to load initial tiled map");
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------------------
//  Main Application Loop
//-------------------------------------------------------------------------------------------------------------
void Application::Play()
{
    // Run the initial game script from the Game Options xml
    if (luaL_dofile(m_pGame->GetLuaState(), m_gameOptions.m_startScript.c_str()) != 0)
        printf("error %s", lua_tostring(m_pGame->GetLuaState(), -1));

    lua_getglobal(m_pGame->GetLuaState(), "LuaInit");
	if (lua_pcall(m_pGame->GetLuaState(), 0, 0, 0) != 0)
	{
	    std::string errorMessage = "Failed to call lua_cfunction: ";
	    errorMessage += lua_tostring(m_pGame->GetLuaState(), -1);
	    LOG_ERROR(errorMessage.c_str());
	}

	m_pGameTimer->Reset();

	double gameTime{ 0 };

    // Main Game Loop
    while (m_running)
    {
		m_pGameTimer->Tick();
		gameTime = m_pGameTimer->GetDeltaTime();
            
        // Get User Input
        m_pInputSystem->ExecuteCommand();

        // Tick the game
        EventManager::Get()->TickUpdate((unsigned long)m_kEventTime);
        m_pGame->Update(gameTime);
    }
}

// Callback function
void Application::QuitAppFunc(EventDataPtr eventData)
{
    m_running = false;
}

void Application::PlayBgMusic(bool play)
{
    m_pAudioSystem->Play(play);
}

void Application::Cleanup()
{
    delete m_pGameTimer;
    m_pGameTimer = nullptr;
    delete m_pInputSystem;
    m_pInputSystem = nullptr;
    delete m_pLuaManager;
    m_pLuaManager = nullptr;
    delete m_pGame;
    m_pGame = nullptr;
    TTF_Quit();
    delete m_pAudioSystem;
    m_pAudioSystem = nullptr;
    Mix_Quit();
    SDL_DestroyRenderer(m_pRen);
    SDL_DestroyWindow(m_pWin);
    SDL_Quit();
    delete m_pResourceCache;
    m_pResourceCache = nullptr;
    delete m_pEventManager;
    m_pEventManager = nullptr;
    delete m_pOperatingSystem;
    m_pOperatingSystem = nullptr;
    Logger::Cleanup();
}
