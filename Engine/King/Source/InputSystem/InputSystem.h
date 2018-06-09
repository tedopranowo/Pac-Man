#pragma once

#include "SDL_InputInterpreter.h"
#include <map>
#include <lua.hpp>
#include "../EventSystem/TypedefEvents.h"

typedef const char* Command;
typedef void(*CommandFunc)(void);
typedef std::map<Command, EventDataPtr> CommandMode;

class Application;

class InputSystem
{
    SDL_InputInterpreter m_inputInterpreter;
    CommandMode* m_pMode;

    CommandMode FreeFlyMode;
    CommandMode GameMode;
    Command m_command;

public:
    InputSystem(Application *pApp);
    ~InputSystem() {}

    void ExecuteCommand();
    Command GetCommand();
};