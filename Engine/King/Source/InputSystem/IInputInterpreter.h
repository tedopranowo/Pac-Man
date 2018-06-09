#pragma once

typedef const char* Command;

class IInputInterpreter
{
public:
    virtual Command GetKeyDown() = 0;
};