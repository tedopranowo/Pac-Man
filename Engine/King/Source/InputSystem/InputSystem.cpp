#include "InputSystem.h"

#include "..\Application\Application.h"
#include "..\..\Source\EventSystem\EventManager.h"
#include "..\..\Source\Events\PanCameraEvent.h"
#include "..\..\Source\Events\SwitchToGameModeEvent.h"
#include "..\..\Source\Events\QuitApplicationEvent.h"
#include "..\..\Source\Events\SwitchToFreeFlyModeEvent.h"

// This function will be called from Lua and will pass in the input system as it's only param
int GetCommandForLua(lua_State* pState)
{
	InputSystem* pInputSystem = static_cast<InputSystem*>(lua_touserdata(pState, -1));

	// Pop the first param
	lua_pop(pState, 1);

	Command command = pInputSystem->GetCommand();

	lua_pushstring(pState, command);

	return 1;
}

InputSystem::InputSystem(Application *pApp)
    : m_inputInterpreter(SDL_InputInterpreter())
    , m_pMode(&GameMode)
    , m_command(nullptr)
{
    // Different modes will call different events depending on the key pressed
#ifdef _DEBUG
	// Free Fly mode is used for to move the camera around freely in the game world (also shows collision boxes)
	FreeFlyMode["1"]        = std::make_shared<SwitchToGameModeEvent>();
	FreeFlyMode["escape"]   = std::make_shared<QuitApplicationEvent>();
	FreeFlyMode["w"]        = std::make_shared<PanCameraEvent>(NORTH);
	FreeFlyMode["s"]        = std::make_shared<PanCameraEvent>(SOUTH);
	FreeFlyMode["a"]        = std::make_shared<PanCameraEvent>(WEST);
	FreeFlyMode["d"]        = std::make_shared<PanCameraEvent>(EAST);

	// This is the Game Mode, the camera and controls will be set to the specific game options
	GameMode["2"]       = std::make_shared<SwitchToFreeFlyMode>();
#endif // DEBUG

	GameMode["escape"] = std::make_shared<QuitApplicationEvent>();

	CREATE_AND_ADD_LUA_DESCRIPTOR(pApp, getCommandDescriptor, "GetCommand", GetCommandForLua);
}

// Get a commmand from the interpreter and execute a commmand based on the current mode
void InputSystem::ExecuteCommand()
{
    m_command = m_inputInterpreter.GetKeyDown();

    auto commandIt = m_pMode->find(m_command);
    if (commandIt != m_pMode->end())
    {
        if (strcmp(commandIt->first, "1") == 0)
        {
            m_pMode = &GameMode;
        }
        else if (strcmp(commandIt->first, "2") == 0)
        {
#ifdef _DEBUG
			m_pMode = &FreeFlyMode;
#endif // DEBUG
        }
        EventManager::Get()->QueueEvent(commandIt->second);
    }
}

Command InputSystem::GetCommand()
{
    if (m_pMode == &GameMode)
    {
        if (m_command != nullptr)
            return m_command;
    }

    return nullptr;
}