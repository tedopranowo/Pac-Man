#include "OpSys.h"

#include "../../Logging/Source/Logger.h"
#include <locale>

OpSys* OpSys::m_pInstance = nullptr;


OpSys::OpSys()
{
}


OpSys::~OpSys()
{
}

// [TODO] switch this to Get()
OpSys* OpSys::Create()
{
    if (m_pInstance == nullptr)
    {
#ifdef _WIN32
        m_pInstance = new WindowsOS();
#endif
    }
    return m_pInstance;
}

void OpSys::Cleanup()
{
    if (m_pInstance)
        delete m_pInstance;
}

bool OpSys::CheckSystemResources()
{
    return false;
}


WindowsOS::WindowsOS()
{
    m_consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

WindowsOS::~WindowsOS()
{
}

bool WindowsOS::CheckFreeDiskSpace(const unsigned long long diskSpaceNeeded)
{
    // Used for save game data space requirement calculations
    DWORD sectorsPerCluster;
    DWORD bytesPerSector;
    DWORD numFreeClusters;
    DWORD totalClusters;

    GetDiskFreeSpace("C:", &sectorsPerCluster, &bytesPerSector, &numFreeClusters, &totalClusters);

    UINT64 NeededClusters = diskSpaceNeeded / (sectorsPerCluster * bytesPerSector);

    if (NeededClusters > numFreeClusters)
    {
       LOG_ERROR("not enough hard disk memory to launch game.");
       return false;
    }

    // return false??
    return true;
}

bool WindowsOS::WindowAlreadyExists(const char * pWindowName)
{
	HWND WindowHandle = FindWindow(nullptr, pWindowName);

	if (WindowHandle != nullptr)
	{
		ShowWindow(WindowHandle, SW_SHOWNORMAL);
		SetFocus(WindowHandle);
		SetForegroundWindow(WindowHandle);
		SetActiveWindow(WindowHandle);
		
		return true;
	}
	
	return false;
}

bool WindowsOS::ReadDevelopmentResourceDirectory(std::string filepath)
{
    HANDLE fileHandle;
    WIN32_FIND_DATA findData;

    fileHandle = FindFirstFile(filepath.c_str(), &findData);

    if (fileHandle != INVALID_HANDLE_VALUE)
    {
        while (FindNextFile(fileHandle, &findData))
        {
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
                continue;

            std::string filename = findData.cFileName;
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (filename != ".." && filename != ".")
                {
                    filename = filepath.substr(0, filepath.length() - 1) +
                        filename + "\\*";
                    ReadDevelopmentResourceDirectory(filename);
                }
            }
            else
            {
                filename = filepath.substr(0, filepath.length() - 1) + filename;
                std::locale locale;

                for (size_t index = 0; index < filename.length(); ++index)
                    filename[index] = std::tolower(filename[index], locale);

                if (GetSearchFilter().empty() || filename.find(GetSearchFilter())
                    != std::string::npos)
                {
                    m_contentMap[filename] = m_assetFileInfo.size();
                    m_assetFileInfo.push_back(findData);
                }
            }
        }
    }

    return (m_assetFileInfo.size() != 0);
}