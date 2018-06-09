#pragma once

#include <fstream>
#include <cassert>

#define LOG(x, y, z) Logger::Log(x, y, z, __FILE__, __LINE__)
#define LOG_INIT(x) Logger::Log("Initialized", x, Logger::CautionLevel::Success, __FILE__, __LINE__)
#define LOG_WARNING(x) Logger::Log("Warning", x, Logger::CautionLevel::Warning, __FILE__, __LINE__)
#define LOG_ERROR(x) Logger::Log("Error", x, Logger::CautionLevel::Error, __FILE__, __LINE__)
#define LOG_EVENT(x) Logger::Log("Event", x, Logger::CautionLevel::None, __FILE__, __LINE__)

class Logger
{
private:
    static bool m_isInitialized;
    static std::ofstream m_fileStream;

public:
    
    enum CautionLevel
    {
        Success = 9, // Blue
        None = 15, // White
        Warning = 14, // Yellow
        Error = 12, // Red
    };

    Logger() {}
    ~Logger() {}

    static const bool Initialize(const char* logPath);
    static void Log(const char* category, const char* message, CautionLevel cautionLevel, const char* file, int line);
    static void Cleanup();
};