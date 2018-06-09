#pragma once

#ifdef _WIN32
    #define WIN_32_LEAN_AND_MEAN
    #include <Windows.h>
#endif

#include <map>
#include <string>
#include <vector>

class OpSys
{
private:
    static OpSys* m_pInstance;
    std::string m_filter;

protected:
    typedef std::map <std::string, size_t> ContentMap;
    ContentMap m_contentMap;

public:
    // Abstract Factory
    OpSys();
    static OpSys* Create();
    virtual ~OpSys();
    static void Cleanup();

    virtual bool CheckFreeDiskSpace(const unsigned long long diskSpaceNeeded) = 0;
    virtual bool CheckSystemResources();

    ContentMap::iterator GetContentMapBegin() { return m_contentMap.begin(); };
    ContentMap::iterator GetContentMapEnd() { return m_contentMap.end(); };

    virtual bool ReadDevelopmentResourceDirectory(std::string FilePath) = 0;
    void SetSearchFilter(char* pFilter) { m_filter = pFilter; }
    const std::string& GetSearchFilter() { return m_filter; }

};

class WindowsOS : public OpSys
{
private:
    HANDLE m_consoleHandle;
    CONSOLE_SCREEN_BUFFER_INFO consoleBufferInfo;
    
    std::vector<WIN32_FIND_DATA> m_assetFileInfo;

public: 
    WindowsOS();
    ~WindowsOS();

	bool WindowAlreadyExists(const char * pWindowName);
    virtual bool CheckFreeDiskSpace(const unsigned long long diskSpaceNeeded) override;
    virtual bool ReadDevelopmentResourceDirectory(std::string filepath) override;
private:
    WIN32_FIND_DATA* GetFile(std::string filePath);
};

