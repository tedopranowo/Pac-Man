#include "../Application/Application.h"

#include "../../SDL2_mixer/include/SDL_mixer.h"

struct SFX
{
    Mix_Chunk *m_sfx;

    SFX(char* filename);
    ~SFX() {}

    void Play();
};

struct BgMusic
{
    Mix_Chunk *m_pBgMusic;
    bool m_musicStarted;
    
    BgMusic(const char* pFilename);
    ~BgMusic() {}

    void Play(bool play);
};

class AudioSystem
{
private:
    BgMusic m_bgMusic;

public:
    AudioSystem(const char* filename ) : m_bgMusic(BgMusic(filename)) {}
    ~AudioSystem() {}

    void Play(bool play);
};