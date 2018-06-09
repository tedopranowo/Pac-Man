#include "Logger.h"
#include <iostream>
#include <Windows.h>

bool Logger::m_isInitialized = false;
std::ofstream Logger::m_fileStream;

const bool Logger::Initialize(const char* logPath)
{
    if (m_isInitialized)
    {
        m_fileStream.close();
    }
    m_fileStream.open(logPath, std::ios::app);

    m_isInitialized = m_fileStream.is_open();

    return m_isInitialized;
}

void Logger::Log(const char* category, const char* message, CautionLevel cautionLevel, const char* file, int line)
{
    if (m_isInitialized)
    {
        HANDLE console;
        console = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(console, cautionLevel);

        m_fileStream << '[' << category << "] " << message << std::endl
            << file << ": Line " << line << std::endl;
        std::cout << '[' << category << "] " << message << std::endl
            << file << ": Line " << line << std::endl;
    }
}

void Logger::Cleanup()
{
    if (m_isInitialized)
    {
        m_fileStream.close();
    }
}